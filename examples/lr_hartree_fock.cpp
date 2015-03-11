#include <memory>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>

#include "../include/tbb.h"
#include "../include/libint.h"
#include "../include/tiledarray.h"
#include "../include/btas.h"

#include "../utility/make_array.h"
#include "../utility/parallel_print.h"
#include "../utility/array_storage.h"
#include "../utility/time.h"
#include "../utility/ta_helpers.h"

#include "../tensor/conversions/tile_pimpl_to_ta_tensor.h"

#include "../molecule/atom.h"
#include "../molecule/cluster.h"
#include "../molecule/molecule.h"
#include "../molecule/clustering_functions.h"
#include "../molecule/make_clusters.h"

#include "../basis/atom_basisset.h"
#include "../basis/basis_set.h"
#include "../basis/cluster_shells.h"
#include "../basis/basis.h"

#include "../integrals/btas_to_ta_tensor.h"
#include "../integrals/btas_to_low_rank_tensor.h"
#include "../integrals/make_engine.h"
#include "../integrals/ta_tensor_to_low_rank_tensor.h"
#include "../integrals/integral_engine_pool.h"
#include "../integrals/sparse_task_integrals.h"
#include "../integrals/dense_task_integrals.h"

#include "../purification/sqrt_inv.h"
#include "../purification/purification_devel.h"


using namespace tcc;
namespace ints = integrals;

template <typename T, unsigned int DIM, typename TileType, typename Policy>
void print_size_info(TiledArray::Array<T, DIM, TileType, Policy> const &a,
                     std::string name) {
    utility::print_par(a.get_world(), "Printing size information for ", name,
                       "\n");

    auto data = utility::array_storage(a);

    utility::print_par(a.get_world(), "\tFull   = ", data[0], " GB\n",
                       "\tSparse = ", data[1], " GB\n", "\tLow Rank = ",
                       data[2], " GB\n");
}

template <typename Pool, typename Basis, unsigned long DIM, typename Fn>
TiledArray::Array<double, DIM, typename Fn::TileType, TiledArray::SparsePolicy>
time_and_print_block_sparse(madness::World &world, Pool &&pool,
                            std::array<Basis, DIM> bs, Fn fn,
                            std::string name) {
    utility::print_par(world, name, "\n");
    auto int_timer = tcc_time::make_timer([=, &world]() {
        return integrals::BlockSparseIntegrals(world, pool, bs, fn);
    });
    auto result = int_timer.apply();
    utility::print_par(world, name, " time = ", int_timer.time(), "\n");
    print_size_info(result, name);
    return result;
}

class Ex_info {
    TiledArray::Array<double, 3, tensor::TilePimpl<double>,
                      TiledArray::SparsePolicy> Xab;
    double threshold;

  public:
    Ex_info(TiledArray::Array<double, 3, TiledArray::Tensor<double>,
                              TiledArray::SparsePolicy> &A,
            double thresh)
        : threshold(thresh) {

        Xab = TiledArray::to_new_tile_type(
            A, integrals::compute_functors::TaToLowRankTensor<3>{threshold});

        utility::print_par(Xab.get_world(), "Xab storage info\n");
        print_size_info(Xab, "Xab");
        Xab.get_world().gop.fence();
    }


    void
    contract_and_print(TiledArray::Array<double, 2, TiledArray::Tensor<double>,
                                         TiledArray::SparsePolicy> const &D) {
        D.get_world().gop.fence(); // Fence to try and promote clean up.

        auto D_lr = TiledArray::to_new_tile_type(
            D, integrals::compute_functors::TaToLowRankTensor<2>{threshold});

        decltype(Xab) X_temp;
        X_temp("X,i,a") = Xab("X,i,j") * D_lr("j,a");
        X_temp.truncate();

        // Recompress X_temp
        using it_t = decltype(Xab.begin());
        Xab.get_world().taskq.for_each(madness::Range<it_t>(X_temp.begin(),
                                                            X_temp.end()),
                                       [=](it_t it) {
            if (it->get().cut() != threshold) {
                throw std::runtime_error("Threshold was not correctly set!");
            }
            it->get().compress();
            return madness::Future<bool>(true);
        });

        utility::print_par(Xab.get_world(), "X_temp storage info\n");
        print_size_info(X_temp, "X_temp");
        D.get_world().gop.fence();
        utility::print_par(Xab.get_world(), "\n");
    }

    auto contract_and_form_K(
        TiledArray::Array<double, 2, TiledArray::Tensor<double>,
                          TiledArray::SparsePolicy> const &D)
        -> std::remove_reference<decltype(D)>::type {
        D.get_world().gop.fence(); // Fence to try and promote clean up.

        auto D_lr = TiledArray::to_new_tile_type(
            D, integrals::compute_functors::TaToLowRankTensor<2>{threshold});

        decltype(Xab) X_temp;
        X_temp("X,a,i") = Xab("X,i,j") * D_lr("j,a");
        X_temp.truncate();

        // Recompress X_temp
        using it_t = decltype(Xab.begin());
        Xab.get_world().taskq.for_each(
            madness::Range<it_t>(X_temp.begin(), X_temp.end()), [](it_t it) {
                it->get().compress();
                return madness::Future<bool>(true);
            });

        TiledArray::Array<double, 2, tensor::TilePimpl<double>,
                          TiledArray::SparsePolicy> K;
        K("i,j") = X_temp("X,a,i") * X_temp("X,a,j");

        auto K_fr = TiledArray::to_new_tile_type(
            K, tensor::tile_pimpl_to_ta_tensor<double>);

        D.get_world().gop.fence();

        return K_fr;
    }
};

int main(int argc, char *argv[]) {
    auto &world = madness::initialize(argc, argv);
    std::string mol_file = "";
    std::string basis_name = "";
    std::string df_basis_name = "";
    int nclusters = 0;
    double threshold = 1e-11;
    double low_rank_threshold = 1e-7;
    volatile int debug = 0;
    if (argc >= 5) {
        mol_file = argv[1];
        basis_name = argv[2];
        df_basis_name = argv[3];
        nclusters = std::stoi(argv[4]);
    } else {
        std::cout << "input is $./program mol_file basis_file df_basis_file "
                     "nclusters \n";
        return 0;
    }
    if (argc == 6) {
        threshold = std::stod(argv[5]);
    }
    if (argc == 7) {
        debug = std::stoi(argv[6]);
    }
    TiledArray::SparseShape<float>::threshold(threshold);
    utility::print_par(world, "Sparse threshold is ",
                       TiledArray::SparseShape<float>::threshold(), "\n");

    auto mol = molecule::read_xyz(mol_file);
    auto charge = 0;
    auto occupation = mol.occupation(charge);
    auto repulsion_energy = mol.nuclear_repulsion();

    utility::print_par(world, "Computing ", mol.nelements(), " elements with ",
                       nclusters, " clusters. Nuclear repulsion energy = ",
                       repulsion_energy, "\n");

    auto clusters = molecule::attach_hydrogens_kmeans(mol, nclusters);

    basis::BasisSet bs{basis_name};
    basis::BasisSet df_bs{df_basis_name};

    basis::Basis basis{bs.create_basis(clusters)};
    basis::Basis df_basis{df_bs.create_basis(clusters)};


    libint2::init();

    // Compute overlap.
    auto overlap_pool = ints::make_pool(ints::make_1body("overlap", basis));
    auto S = time_and_print_block_sparse(
        world, overlap_pool, utility::make_array(basis, basis),
        integrals::compute_functors::BtasToTaTensor{}, "Overlap");

    // Invert overlap
    utility::print_par(world, "\nComputing overlap inverse\n");
    auto overlap_inv_sqrt = pure::inverse_sqrt(S);
    print_size_info(S, "Overlap inverse sqrt");

    // Compute T
    auto kinetic_pool = ints::make_pool(ints::make_1body("kinetic", basis));
    utility::print_par(world, "\n");
    auto T = time_and_print_block_sparse(
        world, kinetic_pool, utility::make_array(basis, basis),
        integrals::compute_functors::BtasToTaTensor{}, "Kinetic");

    // Compute V
    auto nuclear_pool = ints::make_pool(ints::make_1body("nuclear", basis));
    utility::print_par(world, "\n");
    auto V = time_and_print_block_sparse(
        world, nuclear_pool, utility::make_array(basis, basis),
        integrals::compute_functors::BtasToTaTensor{}, "Nuclear");

    // Compute Hcore
    utility::print_par(world, "\nComputing Hcore\n");
    decltype(V) H;
    H("i,j") = T("i,j") + V("i,j");
    print_size_info(H, "Hcore");

    // Compute intial density
    utility::print_par(world, "\nComputing Density\n");
    auto purifier = pure::make_orthogonal_tr_reset_pure(overlap_inv_sqrt);
    auto D = purifier(H, occupation);
    print_size_info(D, "D initial");

    // Begin Two electron integrals section.
    auto eri_pool = ints::make_pool(ints::make_2body(basis, df_basis));

    // Computing Eri2
    utility::print_par(world, "\n");
    auto eri2 = time_and_print_block_sparse(
        world, eri_pool, utility::make_array(df_basis, df_basis),
        integrals::compute_functors::BtasToTaTensor{}, "Eri2 Integrals");

    // Computing the sqrt inverse of Eri2
    utility::print_par(world, "\nComputing eri2 sqrt Inverse\n");
    auto inv_timer
        = tcc_time::make_timer([&]() { return pure::inverse_sqrt(eri2); });
    auto eri2_sqrt_inv = inv_timer.apply();
    utility::print_par(world, "Eri2 inverse computation time = ",
                       inv_timer.time(), "\n");
    print_size_info(eri2_sqrt_inv, "Eri2 sqrt inverse");

    /*
     * Start using Low rank arrays
     */
    // Compute center integrals
    utility::print_par(world, "\n");
    auto Xab = time_and_print_block_sparse(
        world, eri_pool, utility::make_array(df_basis, basis, basis),
        integrals::compute_functors::BtasToTaTensor{}, "Eri3 integrals");


    Xab("X,i,j") = eri2_sqrt_inv("X,P") * Xab("P,i,j");
    Xab.truncate();

    Ex_info lr_info(Xab, low_rank_threshold);


    decltype(D) J, K, F;
    J("i,j") = (Xab("X,a,b") * D("a,b")) * Xab("X,i,j");
    K("i,j") = (Xab("X,i,b") * D("b,a")) * Xab("X,a,j");
    F("i,j") = H("i,j") + 2 * J("i,j") - K("i,j");

    D = purifier(F, occupation);
    auto energy = D("i,j").dot(F("i,j") + H("i,j"), world).get();

    lr_info.contract_and_print(D);

    utility::print_par(world, "\nStarting SCF iterations\n");
    auto diis = TiledArray::DIIS<decltype(D)>{3, 7};
    auto iter = 1;
    decltype(F) Ferror;
    auto error = 1.0;
    const auto volume = double(F.trange().elements().volume());
    while (error >= 1e-12 && iter <= 35) {
        auto t0 = tcc_time::now();

        J("i,j") = (Xab("X,a,b") * D("a,b")) * Xab("X,i,j");
        J.truncate();

        auto k0 = tcc_time::now();
        K("i,j") = (Xab("X,i,b") * D("b,a")) * Xab("X,a,j");
        auto k1 = tcc_time::now();

        F("i,j") = H("i,j") + 2 * J("i,j") - K("i,j");
        Ferror("i,j") = F("i,k") * D("k,l") * S("l,j")
                        - S("i,k") * D("k,l") * F("l,j");
        Ferror.truncate();
        error = Ferror("i,j").norm().get() / volume;
        diis.extrapolate(F, Ferror);

        D = purifier(F, occupation);
        energy = D("i,j").dot(F("i,j") + H("i,j"), world).get();
        world.gop.fence();

        auto t1 = tcc_time::now();

        auto time = tcc_time::duration_in_s(t0, t1);
        auto ktime = tcc_time::duration_in_s(k0, k1);

        utility::print_par(world, "Iteration: ", iter++, " has energy ",
                           std::setprecision(11), energy, " with error ", error,
                           " in ", time, " s with K time ", ktime, "\n");

        lr_info.contract_and_print(D);
    }


    utility::print_par(world, "Final energy = ", std::setprecision(11),
                       energy + repulsion_energy, "\n");

    /* K("i,j") = (Xab("X,i,b") * D("b,a")) * Xab("X,a,j"); */
    /* decltype(K) K_diff; */
    /* decltype(K) K_lr = lr_info.contract_and_form_K(D); */
    /* K_diff("i,j") = K("i,j") - K_lr("i,j"); */

    /* auto norm_diff = K_diff("dont,matter").norm(world).get(); */

    /* utility::print_par(world, "K_diff norm = ", norm_diff, "\n"); */
    /* utility::print_par(world, "K_diff norm/volume = ", */
    /*                    norm_diff / K_diff.elements().volume(), "\n"); */

    world.gop.fence();
    libint2::cleanup();
    madness::finalize();
    return 0;
}