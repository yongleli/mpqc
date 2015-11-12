#include <memory>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "../include/libint.h"
#include "../include/tiledarray.h"
#include "../include/btas.h"

#include "../utility/make_array.h"
#include "../utility/parallel_print.h"
#include "../utility/parallel_break_point.h"
#include "../utility/array_storage.h"
#include "../utility/time.h"
#include "../utility/json_input.h"

#include "../molecule/atom.h"
#include "../molecule/cluster.h"
#include "../molecule/molecule.h"
#include "../molecule/clustering_functions.h"
#include "../molecule/make_clusters.h"

#include "../basis/atom_basisset.h"
#include "../basis/basis_set.h"
#include "../basis/cluster_shells.h"
#include "../basis/basis.h"

#include "../integrals/integrals.h"

#include "../scf/diagonalize_for_coffs.hpp"
#include "../cc/ccsd_t.h"
//#include "../cc/integral_generator.h"
//#include "../cc/lazy_integral.h"
#include "../cc/ccsd_intermediates.h"
#include "../cc/trange1_engine.h"
#include "../ta_routines/array_to_eigen.h"

using namespace mpqc;
namespace ints = integrals;

class ThreeCenterScf {
private:
    using array_type = DArray<2, TA::TensorD, SpPolicy>;
    array_type H_;
    array_type S_;

    array_type F_;
    array_type D_;
    array_type C_;
    array_type L_invV_;
    TiledArray::DIIS<array_type> diis_;

    std::vector<double> k_times_;
    std::vector<double> j_times_;
    std::vector<double> w_times_;
    std::vector<double> scf_times_;

    int64_t occ_;
    double repulsion_;


    void compute_density(int64_t occ) {
        auto F_eig = tcc::array_ops::array_to_eigen(F_);
        auto S_eig = tcc::array_ops::array_to_eigen(S_);

        Eig::GeneralizedSelfAdjointEigenSolver<decltype(S_eig)> es(F_eig,
                                                                   S_eig);
        decltype(S_eig) C = es.eigenvectors().leftCols(occ);
        auto tr_ao = S_.trange().data()[0];

        auto occ_nclusters = (occ_ < 10) ? occ_ : 10;
        auto tr_occ = tcc::scf::tr_occupied(occ_nclusters, occ_);

        C_ = tcc::array_ops::eigen_to_array<TA::TensorD>(H_.get_world(), C, tr_ao, tr_occ);

        D_("i,j") = C_("i,k") * C_("j,k");
    }

    template <typename Integral>
    void form_fock(Integral const &eri3) {
        auto &world = F_.get_world();

        world.gop.fence();
        auto w0 = tcc::utility::time::now();
        TA::Array<double, 3, TA::TensorD, TA::SparsePolicy> W;
        W("X, mu, i") = L_invV_("X,Y") * (eri3("Y, mu, nu") * C_("nu, i"));
        world.gop.fence();
        auto w1 = tcc::utility::time::now();
        w_times_.push_back(tcc::utility::time::duration_in_s(w0,w1));


        array_type J;
        J("mu, nu") = eri3("X, mu, nu")
                      * (L_invV_("Y, X") * (W("Y, rho, i") * C_("rho, i")));
        world.gop.fence();
        auto j1 = tcc::utility::time::now();
        j_times_.push_back(tcc::utility::time::duration_in_s(w1,j1));


        // Permute W
        W("X,i,nu") = W("X,nu,i");
        array_type K;
        K("mu, nu") = W("X, i, mu") * W("X, i, nu");
        world.gop.fence();
        auto k1 = tcc::utility::time::now();
        k_times_.push_back(tcc::utility::time::duration_in_s(j1,k1));

        F_("i,j") = H_("i,j") + 2 * J("i,j") - K("i,j");
    }


public:

    ThreeCenterScf(array_type const &H, array_type const &S,
                   array_type const &L_invV, int64_t occ, double rep)
            : H_(H), S_(S), L_invV_(L_invV), occ_(occ), repulsion_(rep) {
        F_ = H_;
        compute_density(occ_);
    }


    const array_type get_overlap() const {
        return S_;
    }

    const array_type get_fock() const {
        return F_;
    }

    template <typename Integral>
    void solve(int64_t max_iters, double thresh, Integral const &eri3) {
        auto iter = 0;
        auto error = std::numeric_limits<double>::max();
        auto old_energy = 0.0;

        while (iter < max_iters && thresh < error) {
            auto s0 = tcc_time::now();
            F_.get_world().gop.fence();
            form_fock(eri3);

            auto current_energy = energy();
            error = std::abs(old_energy - current_energy);
            old_energy = current_energy;

            array_type Grad;
            Grad("i,j") = F_("i,k") * D_("k,l") * S_("l,j")
                          - S_("i,k") * D_("k,l") * F_("l,j");

            diis_.extrapolate(F_, Grad);

            // Lastly update density
            compute_density(occ_);

            F_.get_world().gop.fence();
            auto s1 = tcc_time::now();
            scf_times_.push_back(tcc_time::duration_in_s(s0, s1));


            std::cout << "Iteration: " << (iter + 1)
            << " energy: " << old_energy << " error: " << error
            << std::endl;
            std::cout << "\tW time: " << w_times_.back() << std::endl;
            std::cout << "\tJ time: " << j_times_.back()
            << " s K time: " << k_times_.back()
            << " s iter time: " << scf_times_.back() << std::endl;

            ++iter;
        }
    }

    double energy() {
        return repulsion_
               + D_("i,j").dot(F_("i,j") + H_("i,j"), D_.get_world()).get();
    }
};

// static auto direct_two_e_ao =
// std::make_shared<tcc::cc::TwoBodyIntGenerator<libint2::Coulomb>>();

static std::map<int, std::string> atom_names = {{1, "H"},
                                                {2, "He"},
                                                {3, "Li"},
                                                {4, "Be"},
                                                {5, "B"},
                                                {6, "C"},
                                                {7, "N"},
                                                {8, "O"},
                                                {9, "F"},
                                                {10, "Ne"},
                                                {11, "Na"},
                                                {12, "Mg"}};


// TODO test case that verify the result automatic
int try_main(int argc, char *argv[], madness::World &world) {


    // parse the input
    rapidjson::Document in;
    parse_input(argc, argv, in);

    std::cout << std::setprecision(15);
    Document cc_in;
    if (in.HasMember("CCSD")){
        cc_in = get_nested(in,"CCSD");
    }
    else if(in.HasMember("CCSD(T)")){
        cc_in = get_nested(in, "CCSD(T)");
    }

    if (!in.HasMember("xyz file") || !in.HasMember("number of bs clusters")
        || !in.HasMember("number of dfbs clusters")
        || !cc_in.HasMember("BlockSize")) {
        if (world.rank() == 0) {
            std::cout << "At a minimum your input file must provide\n";
            std::cout << "\"xyz file\", which is path to an xyz input\n";
            std::cout << "\"number of bs clusters\", which is the number of "
                         "clusters in the obs\n";
            std::cout << "\"number of dfbs clusters\", which is the number of "
                         "clusters in the dfbs\n";
            std::cout << "\"mo block size\", which is the block size for MO "
                         "orbitals\n";
        }
    }

    // declare variables needed for ccsd
    std::shared_ptr<mpqc::cc::CCSDIntermediate<TA::TensorD,TA::SparsePolicy>> intermidiate;

    std::shared_ptr<mpqc::TRange1Engine> tre;

    Eigen::MatrixXd ens;

    TA::Array<double, 2, TA::TensorD, TA::SparsePolicy> fock_mo;

    DArray<4,
            integrals::DirectTile<integrals::IntegralBuilder<4,libint2::TwoBodyEngine<libint2::Coulomb>,integrals::TensorPassThrough>>,
            TA::SparsePolicy> lazy_two_electron_int;
    {

        // Get necessary info
        std::string mol_file = in["xyz file"].GetString();
        int nclusters = in["number of clusters"].GetInt();
        std::size_t blocksize = cc_in["BlockSize"].GetInt();


        // Get basis info
        std::string basis_name = in.HasMember("basis") ? in["basis"].GetString()
                                                       : "cc-pvdz";
        std::string df_basis_name = in.HasMember("df basis")
                                          ? in["df basis"].GetString()
                                          : "cc-pvdz-ri";

        // Get thresh info
        auto threshold = in.HasMember("block sparse threshold")
                               ? in["block sparse threshold"].GetDouble()
                               : 1e-13;

        // get other info
        bool frozen_core = cc_in.HasMember("FrozenCore")
                                 ? cc_in["FrozenCore"].GetBool()
                                 : false;

        if (world.rank() == 0) {
            std::cout << "Mol file is " << mol_file << std::endl;
            std::cout << "basis is " << basis_name << std::endl;
            std::cout << "df basis is " << df_basis_name << std::endl;
            std::cout << "Using " << nclusters << " clusters"
                      << std::endl;
        }

        TiledArray::SparseShape<float>::threshold(threshold);
        tcc::utility::print_par(world, "Sparse threshold is ",
                           TiledArray::SparseShape<float>::threshold(), "\n");

        auto mol = mpqc::molecule::read_xyz(mol_file);
        auto charge = 0;
        auto occ = mol.occupation(charge);
        auto repulsion_energy = mol.nuclear_repulsion();
        auto core_electron = mol.core_electrons();

        tcc::utility::print_par(world, "Nuclear repulsion_energy = ",
                           repulsion_energy, "\n");


        world.gop.fence();

        auto clustered_mol = mpqc::molecule::attach_hydrogens_and_kmeans(mpqc::molecule::read_xyz(mol_file).clusterables(), nclusters);

        mpqc::basis::BasisSet bs{basis_name};
        mpqc::basis::BasisSet df_bs{df_basis_name};

        std::streambuf *cout_sbuf
              = std::cout.rdbuf(); // Silence libint printing.
        std::ofstream fout("/dev/null");
        std::cout.rdbuf(fout.rdbuf());
        mpqc::basis::Basis basis{bs.get_cluster_shells(clustered_mol)};
        mpqc::basis::Basis df_basis{df_bs.get_cluster_shells(clustered_mol)};
        std::cout.rdbuf(cout_sbuf);

        // TODO better basis TRange
        if (world.rank() == 0) {
            std::cout << "Basis trange " << std::endl;
            TA::TiledRange1 bs_range = basis.create_trange1();
            std::cout << bs_range << std::endl;
            TA::TiledRange1 dfbs_range = df_basis.create_trange1();
            std::cout << "DF Basis trange " << std::endl;
            std::cout << dfbs_range << std::endl;
        }

        // start SCF
        libint2::init();

        const auto bs_array = tcc::utility::make_array(basis, basis);

        // Overlap ints
        auto overlap_e = ints::make_1body_shr_pool("overlap", basis, clustered_mol);
        auto S = ints::sparse_integrals(world, overlap_e, bs_array);

        // Overlap ints
        auto kinetic_e = ints::make_1body_shr_pool("kinetic", basis, clustered_mol);
        auto T = ints::sparse_integrals(world, kinetic_e, bs_array);

        auto nuclear_e = ints::make_1body_shr_pool("nuclear", basis, clustered_mol);
        auto V = ints::sparse_integrals(world, nuclear_e, bs_array);

        decltype(T) H;
        H("i,j") = T("i,j") + V("i,j");

        const auto dfbs_array = tcc::utility::make_array(df_basis, df_basis);
        auto eri_e = ints::make_2body_shr_pool(df_basis, basis);

        decltype(H) L_inv;
        {
            auto Vmetric = ints::sparse_integrals(world, eri_e, dfbs_array);
            auto V_eig = tcc::array_ops::array_to_eigen(Vmetric);
            MatrixD Leig = Eig::LLT<MatrixD>(V_eig).matrixL();
            MatrixD L_inv_eig = Leig.inverse();

            auto tr_V = Vmetric.trange().data()[0];
            L_inv = tcc::array_ops::eigen_to_array<TA::TensorD>(world, L_inv_eig,
                                                                tr_V, tr_V);
        }

        auto three_c_array = tcc::utility::make_array(df_basis, basis, basis);
        auto eri3 = ints::sparse_integrals(world, eri_e, three_c_array);
        ThreeCenterScf scf(H, S, L_inv, occ / 2, repulsion_energy);
        scf.solve(20, 1e-7, eri3);

        // end SCF


        // start ccsd prepration
        auto n_occ = occ / 2;

        tcc::utility::print_par(world, "\nCC Calculation\n");

        int n_frozen_core = 0;
        if (frozen_core) {
            n_frozen_core = mol.core_electrons();
            tcc::utility::print_par(world, "Frozen Core: ", n_frozen_core,
                               " electrons", "\n");
            n_frozen_core = n_frozen_core / 2;
        }

        TA::Array<double,2,TA::TensorD,TA::SparsePolicy> F;
        F = scf.get_fock();

        TA::Array<double,3,TA::TensorD, TA::SparsePolicy> Xab;
        Xab("X,a,b") = L_inv("X,Y")*eri3("Y,a,b");



        auto F_eig = tcc::array_ops::array_to_eigen(F);
        auto S_eig = tcc::array_ops::array_to_eigen(S);
        Eig::GeneralizedSelfAdjointEigenSolver<decltype(S_eig)> es(F_eig, S_eig);

        ens = es.eigenvalues().bottomRows(S_eig.rows() - n_frozen_core);

        auto C_all = es.eigenvectors();
        decltype(S_eig) C_occ = C_all.block(0, n_frozen_core, S_eig.rows(), occ / 2 - n_frozen_core);
        decltype(S_eig) C_vir = C_all.rightCols(S_eig.rows() - occ / 2);
        C_all = C_all.rightCols(S_eig.rows() - n_frozen_core);

        std::size_t all = S.trange().elements().extent()[0];

        tre = std::make_shared<TRange1Engine>(occ / 2, all, blocksize, n_frozen_core);

        auto tr_0 = Xab.trange().data().back();
        auto tr_all = tre->get_all_tr1();
        auto tr_i0 = tre->get_occ_tr1();
        auto tr_vir = tre->get_vir_tr1();

        tcc::utility::print_par(world, "Block Size in MO     ", blocksize, "\n");
        tcc::utility::print_par(world, "TiledRange1 Occupied ", tr_i0, "\n");
        tcc::utility::print_par(world, "TiledRange1 Virtual  ", tr_vir, "\n");

        auto Ci = tcc::array_ops::eigen_to_array<TA::Tensor<double>>(world, C_occ,
                                                                tr_0, tr_i0);

        auto Cv = tcc::array_ops::eigen_to_array<TA::Tensor<double>>(world, C_vir,
                                                                tr_0, tr_vir);

        auto Call = tcc::array_ops::eigen_to_array<TA::Tensor<double>>(world, C_all,
                                                                  tr_0, tr_all);

        std::vector<TA::TiledRange1> tr_04(4, tr_0);
        TA::TiledRange trange_4(tr_04.begin(), tr_04.end());

        auto screen_builder = ints::init_schwarz_screen(1e-10);
        auto shr_screen = std::make_shared<ints::SchwarzScreen>(screen_builder(world, eri_e, basis));

        const auto bs4_array = tcc::utility::make_array(basis, basis,basis,basis);
        lazy_two_electron_int = mpqc_ints::direct_sparse_integrals(world, eri_e, bs4_array, shr_screen);

        world.gop.fence();

        intermidiate = std::make_shared<mpqc::cc::CCSDIntermediate<TA::TensorD, TA::SparsePolicy>>
            (Xab, Ci, Cv, lazy_two_electron_int);


        fock_mo("p,q") = F("mu,nu") * Cv("mu,p") * Ci("nu,q");
    }

    // clean up all temporary from HF
    world.gop.fence();

    tcc::utility::print_par(world, "\nBegining CC Calculation\n");
    tcc::utility::parallal_break_point(world, 0);


    if(in.HasMember("CCSD(T)")){
        mpqc::cc::CCSD_T<TA::Tensor < double>, TA::SparsePolicy > ccsd_t(fock_mo, ens, tre, intermidiate, cc_in);
        ccsd_t.compute();
    }
    else if(in.HasMember("CCSD")){
        std::cout << fock_mo << std::endl;
        mpqc::cc::CCSD<TA::Tensor < double>, TA::SparsePolicy > ccsd(fock_mo, ens, tre, intermidiate, cc_in);
        ccsd.compute();
    }

    world.gop.fence();
    libint2::cleanup();
    return 0;
}


int main(int argc, char *argv[]) {

    int rc = 0;

    auto &world = madness::initialize(argc, argv);

    try {

        try_main(argc, argv, world);

    } catch (TiledArray::Exception &e) {
        std::cerr << "!! TiledArray exception: " << e.what() << "\n";
        rc = 1;
    } catch (madness::MadnessException &e) {
        std::cerr << "!! MADNESS exception: " << e.what() << "\n";
        rc = 1;
    } catch (SafeMPI::Exception &e) {
        std::cerr << "!! SafeMPI exception: " << e.what() << "\n";
        rc = 1;
    } catch (std::exception &e) {
        std::cerr << "!! std exception: " << e.what() << "\n";
        rc = 1;
    } catch (...) {
        std::cerr << "!! exception: unknown exception\n";
        rc = 1;
    }


    madness::finalize();
    return rc;
}
