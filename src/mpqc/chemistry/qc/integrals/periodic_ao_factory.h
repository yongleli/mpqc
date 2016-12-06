#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_PERIODIC_AO_FACTORY_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_PERIODIC_AO_FACTORY_H_

#include "mpqc/chemistry/qc/integrals/ao_factory_base.h"

#include <iosfwd>
#include <vector>

#include "mpqc/chemistry/molecule/unit_cell.h"
#include "mpqc/chemistry/qc/integrals/integrals.h"
#include "mpqc/math/external/eigen/eigen.h"
#include "mpqc/math/tensor/clr/array_to_eigen.h"
#include "mpqc/util/keyval/keyval.h"
#include "mpqc/util/misc/time.h"
#include "mpqc/chemistry/units/units.h"

#include <unsupported/Eigen/MatrixFunctions>

typedef Eigen::Matrix<std::complex<double>, Eigen::Dynamic, Eigen::Dynamic,
                      Eigen::RowMajor>
    Matrixz;
typedef Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1> Vectorz;

// constant
const std::complex<double> I(0.0, 1.0);

namespace mpqc {
namespace integrals {

template <typename Tile, typename Policy>
class PeriodicAOFactory;

namespace detail {

/*!
 * \brief This constructs a PeriodicAOFactory object
 *
 * \tparam Tile the type of TA Tensor
 * \tparam Policy the type of TA policy
 * \param kv KeyVal object
 * \return the shared pointer of PeriodicAOFactory object
 */
template <typename Tile, typename Policy>
std::shared_ptr<PeriodicAOFactory<Tile, Policy>> construct_periodic_ao_factory(
    const KeyVal &kv) {
  std::shared_ptr<PeriodicAOFactory<Tile, Policy>> pao_factory;

  if (kv.exists_class("wfn_world:periodic_ao_factory")) {
    pao_factory = kv.class_ptr<PeriodicAOFactory<Tile, Policy>>(
        "wfn_world:periodic_ao_factory");
  } else {
    pao_factory = std::make_shared<PeriodicAOFactory<Tile, Policy>>(kv);
    std::shared_ptr<DescribedClass> pao_factory_base = pao_factory;
    KeyVal &kv_nonconst = const_cast<KeyVal &>(kv);
    kv_nonconst.keyval("wfn_world")
        .assign("periodic_ao_factory", pao_factory_base);
  }
  return pao_factory;
}

/*!
 * \brief This extends 1D tiled range by repeating it multiple times
 *
 * \param tr0 the original TiledRange1 object
 * \param size the number of times for repeating
 * \return the extended TiledRange1 object
 */
TA::TiledRange1 extend_trange1(TA::TiledRange1 tr0, int64_t size);

/*!
 * \brief This sorts eigenvalues and eigenvectors
 * in ascending order of the real parts of eigenvalues
 *
 * \param eigVal the vector of complex eigenvalues
 * \param eigVec the complex matrix consisting of complex eigenvectors
 */
void sort_eigen(Vectorz &eigVal, Matrixz &eigVec);

/*!
 * \brief This takes the ordinal index of a lattice
 * and returns the corresponding direct-space lattice vector
 *
 * \param ord_index the ordinal index of the lattice
 * \param latt_max the range of included lattices
 * \param dcell the direct unit cell params
 * \return the direct-space lattice vector
 */
Vector3d direct_vector(int64_t ord_idx, Vector3i latt_max, Vector3d dcell);

/*!
 * \brief This takes the ordinal index of a k-space (reciprocal-space) lattice
 * and returns the corresponding k-space lattice vector
 *
 * \param ord_idx the ordinal index of the reciprocal lattice
 * \param nk the range of included k points
 * \param dcell the direct unit cell params
 * \return the k-space lattice vector
 */
Vector3d k_vector(int64_t ord_idx, Vector3i nk, Vector3d dcell);

/*!
 * \brief This takes the 3D index of a direct lattice
 * and returns the corresponding ordinal index
 *
 * \param x the direct lattice index on x axis
 * \param y the direct lattice index on y axis
 * \param z the direct lattice index on z axis
 * \param latt_max the range of included lattices
 * \return the ordinal index in direct space
 */
int64_t direct_ord_idx(int64_t x, int64_t y, int64_t z, Vector3i latt_max);

/*!
 * \brief This takes the 3D index of a reciprocal lattice
 * and returns the corresponding ordinal index
 *
 * \param x the reciprocal lattice index on x' axis
 * \param y the reciprocal lattice index on y' axis
 * \param z the reciprocal lattice index on z' axis
 * \param nk the range of included k points
 * \return the ordinal index in k space
 */
int64_t k_ord_idx(int64_t x, int64_t y, int64_t z, Vector3i nk);

/*!
 * \brief This shifts the origin of a Basis object
 *
 * \note All functions in the basis will be shifted
 * \param basis the original Basis object
 * \param shift the 3D vector of the shift
 * \return the shared pointer of shifted Basis object
 */
std::shared_ptr<basis::Basis> shift_basis_origin(basis::Basis &basis,
                                                 Vector3d shift);

/*!
 * \brief This shifts the origin of a Basis object by multiple vectors,
 * and returns a compound Basis that combines all shifted bases
 *
 * \param basis the original Basis object
 * \param shift_base the base position where all shifting vectors start
 * \param nshift the range of included lattices
 * \param dcell the direct unit cell params
 * \return the shared pointer of the compound Basis object
 */
std::shared_ptr<basis::Basis> shift_basis_origin(basis::Basis &basis,
                                                 Vector3d shift_base,
                                                 Vector3i nshift,
                                                 Vector3d dcell);

/*!
 * \brief This shifts the position of a Molecule object
 *
 * \note All atom positions will be shifted
 * \param mol the Molecule object
 * \param shift the 3D vector of the shift
 * \return the shared pointer of the shifted Molecule object
 */
std::shared_ptr<Molecule> shift_mol_origin(const Molecule &mol, Vector3d shift);

/*!
 * \brief This gives a valid libint2::any object
 *
 * \param mpqc_oper the type of a MPQC Operator
 * \param base an AOFactoryBase object
 * \param mol should be Molecule/UnitCell type that can provide unitcell info
 * \return libint2::any object
 */
libint2::any to_libint2_operator_params(Operator::Type mpqc_oper,
                                        const AOFactoryBase &base,
                                        const Molecule &mol);

}  // namespace detail


template <typename Tile, typename Policy>
class PeriodicAOFactory : public DescribedClass {
 public:
  using TArray = TA::DistArray<Tile, Policy>;
  using Op = std::function<Tile(TA::TensorZ &&)>;

  PeriodicAOFactory() = default;
  PeriodicAOFactory(PeriodicAOFactory &&) = default;
  PeriodicAOFactory &operator=(PeriodicAOFactory &&) = default;

  /*!
   * \brief KeyVal constructor for PeriodicAOFactory
   * \param kv the KeyVal object
   */
  PeriodicAOFactory(const KeyVal &kv)
      : world_(*kv.value<madness::World*>("$:world")) {
    ao_factory_base_ = std::make_shared<AOFactoryBase>(AOFactoryBase(kv));

    std::string prefix = "";
    if (kv.exists_class("wfn_world"))
      prefix = "wfn_world:";

    std::string molecule_type = kv.value<std::string>(prefix + "molecule:type");
    if (molecule_type != "UnitCell") {
      throw std::invalid_argument("molecule:type has to be UnitCell in order to run PRHF!!");
    }

    unitcell_ = kv.keyval(prefix + "molecule").class_ptr<UnitCell>();
    dcell_ = unitcell_->dcell();

    R_max_ = decltype(R_max_)(
        kv.value<std::vector<int>>(prefix + "molecule:rmax").data());
    RD_max_ = decltype(RD_max_)(
        kv.value<std::vector<int>>(prefix + "molecule:rdmax").data());
    RJ_max_ = decltype(RJ_max_)(
        kv.value<std::vector<int>>(prefix + "molecule:rjmax").data());
    nk_ = decltype(nk_)(
        kv.value<std::vector<int>>(prefix + "molecule:k_points").data());

    R_size_ = 1 + detail::direct_ord_idx(R_max_(0), R_max_(1), R_max_(2), R_max_);
    RJ_size_ = 1 + detail::direct_ord_idx(RJ_max_(0), RJ_max_(1), RJ_max_(2), RJ_max_);
    RD_size_ = 1 + detail::direct_ord_idx(RD_max_(0), RD_max_(1), RD_max_(2), RD_max_);
    k_size_ = 1 + detail::k_ord_idx(nk_(0) - 1, nk_(1) - 1, nk_(2) - 1, nk_);

    op_ = TA::Noop<TA::TensorZ, true>();

    print_detail_ = kv.value<bool>("print_detail", false);
  }

  ~PeriodicAOFactory() noexcept = default;

  /// wrapper to compute function
  TArray compute(const std::wstring &);

  /*!
   * \brief This computes integral by Formula
   *
   * This function will look into registry first
   * if Formula computed, it will return it from registry
   * if not, it will compute it
   * \param formula the desired Formula type
   * \return the TA::DistArray object
   */
  TArray compute(const Formula &formula);

  /*!
   * \brief This transforms an integral matrix from real to reciprocal space
   * \param matrix the real-space integral matrix
   * \return the reciprocal-space integral matrix
   */
  TArray transform_real2recip(TArray &matrix);

  /// @return the range of expansion of Bloch Gaussians in AO Gaussians
  Vector3i R_max() { return R_max_; }

  /// @return the range of Coulomb operation
  Vector3i RJ_max() { return RJ_max_; }

  /// @return the range of density representation
  Vector3i RD_max() { return RD_max_; }

  /// @return the direct unit cell params
  Vector3i nk() { return nk_; }

  /// @return the cardinal number of lattices included in Bloch Gaussian expansion
  int64_t R_size() { return R_size_; }

  /// @return the cardinal number of lattices included in Coulomb operation
  int64_t RJ_size() { return RJ_size_; }

  /// @return the cardinal number of lattices included in density representation
  int64_t RD_size() { return RD_size_; }

  /// @return the cardinal number of k points
  int64_t k_size() { return k_size_; }

  /*!
   * \brief This sets the density for coulomb and exchange computations
   * in PeriodicAOFactory
   *
   * \param D is the density feeded to PeriodicAOFactory
   */
  void set_density(TArray D) { D_ = D; }

  /// @return MADNESS world
  madness::World &world() { return world_; }

  /// @return AOFactoryBase
  std::shared_ptr<AOFactoryBase> ao_factory_base() {return ao_factory_base_;}

  /// set OrbitalBasisRegistry
  void set_orbital_basis_registry(
      const std::shared_ptr<basis::OrbitalBasisRegistry> &obs) {
    ao_factory_base_->set_orbital_basis_registry(obs);
  }

  /// @return the OrbitalBasisRegistry object
  const basis::OrbitalBasisRegistry &orbital_basis_registry() const {
    return ao_factory_base_->orbital_basis_registry();
  }

  /// @return the OrbitalBasisRegistry object
  basis::OrbitalBasisRegistry &orbital_basis_registry() {
    return ao_factory_base_->orbital_basis_registry();
  }

  /*!
   * \brief This computes sparse complex array
   *
   * \param world MADNESS world
   * \param engine integrals::EnginePool
   * that is initialized with Operator and bases
   * \param bases std::array of Basis
   * \param p_screen Screener
   * \return the integral sparse array
   */
  template <typename U = Policy>
  TA::DistArray<
      Tile, typename std::enable_if<std::is_same<U, TA::SparsePolicy>::value,
                                    TA::SparsePolicy>::type>
  compute_integrals(
      madness::World &world, ShrPool<libint2::Engine> &engine,
      Bvector const &bases,
      std::shared_ptr<Screener> p_screen =
          std::make_shared<integrals::Screener>(integrals::Screener{})) {
    integrals::detail::integral_engine_precision = 0.0;
    auto result = sparse_complex_integrals(world, engine, bases, p_screen, op_);
    return result;
  }


 private:

  /// parse one body formula and set engine_pool and basis array for periodic
  /// system
  void parse_one_body_periodic(
      const Formula &formula,
      std::shared_ptr<EnginePool<libint2::Engine>> &engine_pool, Bvector &bases,
      const Molecule &shifted_mol);

  /// parse two body formula and set engine_pool and basis array for periodic
  /// system
  void parse_two_body_periodic(
      const Formula &formula,
      std::shared_ptr<EnginePool<libint2::Engine>> &engine_pool, Bvector &bases,
      Vector3d shift_coul, bool if_coulomb);

  /*!
   * \brief Construct sparse complex integral tensors in parallel.
   *
   * \param shr_pool should be a std::shared_ptr to an IntegralEnginePool
   * \param bases should be a std::array of Basis, which will be copied.
   * \param op needs to be a function or functor that takes a TA::TensorZ && and
   * returns any valid tile type. Op is copied so it can be moved.
   * ```
   * auto t = [](TA::TensorZ &&ten){return std::move(ten);};
   * ```
   *
   * \param screen should be a std::shared_ptr to a Screener.
   */
  template <typename E>
  TA::DistArray<Tile, TA::SparsePolicy> sparse_complex_integrals(
      madness::World &world, ShrPool<E> shr_pool, Bvector const &bases,
      std::shared_ptr<Screener> screen = std::make_shared<Screener>(Screener{}),
      std::function<Tile(TA::TensorZ &&)> op = TA::Noop<TA::TensorZ, true>());

  std::shared_ptr<UnitCell> unitcell_;  ///> UnitCell private member
  std::shared_ptr<AOFactoryBase> ao_factory_base_;  ///> AOFactoryBase private member
  madness::World &world_;

  Op op_;

  TArray D_;  ///> Density

  Vector3i R_max_ = {
      0, 0, 0};  ///> range of expansion of Bloch Gaussians in AO Gaussians
  Vector3i RJ_max_ = {0, 0, 0};       ///> range of Coulomb operation
  Vector3i RD_max_ = {0, 0, 0};       ///> range of density representation
  Vector3i nk_ = {1, 1, 1};           ///> # of k points in each direction
  Vector3d dcell_ = {0.0, 0.0, 0.0};  ///> direct unit cell params (in a.u.)

  int64_t R_size_;  ///> cardinal # of lattices included in Bloch Gaussian expansion
  int64_t RJ_size_;  ///> cardinal # of lattices included in Coulomb operation
  int64_t RD_size_;  ///> cardinal # of lattices included in density representation
  int64_t k_size_;  ///> cardinal # of k points

  bool print_detail_; ///> if true, print a lot more details
};

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute(const std::wstring &formula_string) {
  auto formula = Formula(formula_string);
  return compute(formula);
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute(const Formula &formula) {
  TArray result;
  Bvector bs_array;
  std::shared_ptr<EnginePool<libint2::Engine>> engine_pool;
  double size = 0.0;

  if (formula.rank() == 2) {
    utility::print_par(world_,
                       "\nComputing One Body Integral for Periodic System: ",
                       utility::to_string(formula.string()), "\n");

    auto time0 = mpqc::now(world_, false);
    if (formula.oper().type() == Operator::Type::Kinetic ||
        formula.oper().type() == Operator::Type::Overlap) {
      parse_one_body_periodic(formula, engine_pool, bs_array, ao_factory_base_->molecule());
      result = compute_integrals(world_, engine_pool, bs_array);
    } else if (formula.oper().type() == Operator::Type::Nuclear) {
      for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
        auto shift_mol = detail::direct_vector(RJ, RJ_max_, dcell_);
        auto shifted_mol = detail::shift_mol_origin(*unitcell_, shift_mol);
        parse_one_body_periodic(formula, engine_pool, bs_array, *shifted_mol);
        if (RJ == 0)
          result = compute_integrals(world_, engine_pool, bs_array);
        else
          result("mu, nu") +=
              compute_integrals(world_, engine_pool, bs_array)("mu, nu");
      }
    } else
      throw std::runtime_error("Rank-2 operator type not supported");
    auto time1 = mpqc::now(world_, false);
    auto time = mpqc::duration_in_s(time0, time1);

    size = mpqc::detail::array_size(result);
    utility::print_par(world_, " Size: ", size, " GB");
    utility::print_par(world_, " Time: ", time, " s\n");

  } else if (formula.rank() == 4) {
    auto time_4idx = 0.0;
    auto time_contr = 0.0;
    auto time = 0.0;

    if (print_detail_) {
      utility::print_par(world_,
                         "\nComputing Two Body Integral for Periodic System: ",
                         utility::to_string(formula.string()), "\n");
    }

    if (formula.oper().type() == Operator::Type::J) {
      auto time_j0 = mpqc::now(world_, false);

      auto j_formula = formula;
      j_formula.set_operator_type(Operator::Type::Coulomb);

      for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
        auto vec_RJ = detail::direct_vector(RJ, RJ_max_, dcell_);
        parse_two_body_periodic(j_formula, engine_pool, bs_array, vec_RJ, true);

        auto time_g0 = mpqc::now(world_, false);
        auto J = compute_integrals(world_, engine_pool, bs_array);
        auto time_g1 = mpqc::now(world_, false);
        time_4idx += mpqc::duration_in_s(time_g0, time_g1);

        auto time_contr0 = mpqc::now(world_, false);
        if (RJ == 0)
          result("mu, nu") = J("mu, nu, lambda, rho") * D_("lambda, rho");
        else
          result("mu, nu") += J("mu, nu, lambda, rho") * D_("lambda, rho");
        auto time_contr1 = mpqc::now(world_, false);
        time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
      }
      auto time_j1 = mpqc::now(world_, false);
      time = mpqc::duration_in_s(time_j0, time_j1);

    } else if (formula.oper().type() == Operator::Type::K) {
      auto time_k0 = mpqc::now(world_, false);

      auto k_formula = formula;
      k_formula.set_operator_type(Operator::Type::Coulomb);
      for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
        auto vec_RJ = detail::direct_vector(RJ, RJ_max_, dcell_);
        parse_two_body_periodic(k_formula, engine_pool, bs_array, vec_RJ,
                                false);
        auto time_g0 = mpqc::now(world_, false);
        auto K = compute_integrals(world_, engine_pool, bs_array);
        auto time_g1 = mpqc::now(world_, false);
        time_4idx += mpqc::duration_in_s(time_g0, time_g1);

        auto time_contr0 = mpqc::now(world_, false);
        if (RJ == 0)
          result("mu, nu") = K("mu, lambda, nu, rho") * D_("lambda, rho");
        else
          result("mu, nu") += K("mu, lambda, nu, rho") * D_("lambda, rho");
        auto time_contr1 = mpqc::now(world_, false);
        time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
      }
      auto time_k1 = mpqc::now(world_, false);
      time = mpqc::duration_in_s(time_k0, time_k1);

    } else
      throw std::runtime_error("Rank-4 operator type not supported");

    if (print_detail_) {
      size = mpqc::detail::array_size(result);
      utility::print_par(world_, " Size: ", size, " GB\n");
      utility::print_par(world_, " \t4-index g tensor time: ", time_4idx,
                         " s\n");
      utility::print_par(world_, " \tg*D contraction time: ", time_contr,
                         " s\n");
      utility::print_par(world_, " \ttotal time: ", time, " s\n");
    }

  } else
    throw std::runtime_error("Operator rank not supported");

  return result;
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_one_body_periodic(const Formula &formula,
    std::shared_ptr<EnginePool<libint2::Engine>> &engine_pool, Bvector &bases,
    const Molecule &shifted_mol) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 1);
  TA_ASSERT(ket_indices.size() == 1);

  auto bra_index = bra_indices[0];
  auto ket_index = ket_indices[0];

  TA_ASSERT(bra_index.is_ao());
  TA_ASSERT(ket_index.is_ao());

  auto bra_basis = ao_factory_base_->index_to_basis(bra_index);
  auto ket_basis = ao_factory_base_->index_to_basis(ket_index);

  TA_ASSERT(bra_basis != nullptr);
  TA_ASSERT(ket_basis != nullptr);

  // Form a compound ket basis by shifting origins from -Rmax to Rmax
  Vector3d zero_shift_base(0.0, 0.0, 0.0);
  ket_basis = detail::shift_basis_origin(*ket_basis, zero_shift_base, R_max_, dcell_);

  bases = Bvector{{*bra_basis, *ket_basis}};

  auto oper_type = formula.oper().type();
  engine_pool = integrals::make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(*bra_basis, *ket_basis), libint2::BraKet::x_x,
      detail::to_libint2_operator_params(oper_type, *ao_factory_base_, shifted_mol));
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_two_body_periodic(
    const Formula &formula,
    std::shared_ptr<EnginePool<libint2::Engine>> &engine_pool, Bvector &bases,
    Vector3d shift_coul, bool if_coulomb) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 2);
  TA_ASSERT(ket_indices.size() == 2);

  auto bra_index0 = bra_indices[0];
  auto bra_index1 = bra_indices[0];
  auto ket_index0 = ket_indices[0];
  auto ket_index1 = ket_indices[0];

  TA_ASSERT(bra_index0.is_ao());
  TA_ASSERT(bra_index1.is_ao());
  TA_ASSERT(ket_index0.is_ao());
  TA_ASSERT(ket_index1.is_ao());

  auto bra_basis0 = ao_factory_base_->index_to_basis(bra_index0);
  auto bra_basis1 = ao_factory_base_->index_to_basis(bra_index1);
  auto ket_basis0 = ao_factory_base_->index_to_basis(ket_index0);
  auto ket_basis1 = ao_factory_base_->index_to_basis(ket_index1);

  TA_ASSERT(bra_basis0 != nullptr);
  TA_ASSERT(bra_basis1 != nullptr);
  TA_ASSERT(ket_basis0 != nullptr);
  TA_ASSERT(ket_basis1 != nullptr);

  // Form a compound index basis
  Vector3d zero_shift_base(0.0, 0.0, 0.0);
  if (if_coulomb) {
    bra_basis1 = detail::shift_basis_origin(*bra_basis1, zero_shift_base, R_max_, dcell_);
    ket_basis0 = detail::shift_basis_origin(*ket_basis0, shift_coul);
  } else {
    bra_basis1 = detail::shift_basis_origin(*bra_basis1, shift_coul);
    ket_basis0 = detail::shift_basis_origin(*ket_basis0, zero_shift_base, R_max_, dcell_);
  }
  ket_basis1 = detail::shift_basis_origin(*ket_basis1, shift_coul, RD_max_, dcell_);

  if (formula.notation() == Formula::Notation::Chemical)
    bases = Bvector{{*bra_basis0, *bra_basis1, *ket_basis0, *ket_basis1}};
  else
    throw "Physical notation not supported!";
  //    bases = Bvector{{*bra_basis0, *ket_basis0, *bra_basis1, *ket_basis1}};

  auto oper_type = formula.oper().type();
  engine_pool = integrals::make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(bases[0], bases[1], bases[2], bases[3]),
      libint2::BraKet::xx_xx,
      detail::to_libint2_operator_params(oper_type, *ao_factory_base_, *unitcell_));
}

template <typename Tile, typename Policy>
template <typename E>
TA::DistArray<Tile, TA::SparsePolicy>
PeriodicAOFactory<Tile, Policy>::sparse_complex_integrals(
    madness::World &world, ShrPool<E> shr_pool, Bvector const &bases,
    std::shared_ptr<Screener> screen, std::function<Tile(TA::TensorZ &&)> op) {
  // Build the Trange and Shape Tensor
  auto trange = detail::create_trange(bases);
  const auto tvolume = trange.tiles_range().volume();
  std::vector<std::pair<unsigned long, Tile>> tiles(tvolume);
  TA::TensorF tile_norms(trange.tiles_range(), 0.0);

  // Copy the Bases for the Integral Builder
  auto shr_bases = std::make_shared<Bvector>(bases);

  // Make a pointer to an Integral builder.  Doing this because we want to use
  // it in Tasks.
  auto builder_ptr = std::make_shared<IntegralBuilder<Tile, E>>(
      std::move(shr_pool), std::move(shr_bases), std::move(screen),
      std::move(op));

  auto task_f = [=](int64_t ord, detail::IdxVec idx, TA::Range rng,
                    TA::TensorF *tile_norms_ptr, Tile *out_tile) {

    // This is why builder was made into a shared_ptr.
    auto &builder = *builder_ptr;
    auto ta_tile = builder.integrals(idx, std::move(rng));

    const auto tile_volume = ta_tile.range().volume();
    const auto tile_norm = ta_tile.norm();

    // Keep tile if it was significant.
    bool save_norm =
        tile_norm >= tile_volume * TA::SparseShape<float>::threshold();
    if (save_norm) {
      *out_tile = builder.op(std::move(ta_tile));

      auto &norms = *tile_norms_ptr;
      norms[ord] = tile_norm;
    }
  };

  auto pmap = TA::SparsePolicy::default_pmap(world, tvolume);

  auto time_f0 = mpqc::now(world_, true);
  for (auto const ord : *pmap) {
    tiles[ord].first = ord;
    detail::IdxVec idx = trange.tiles_range().idx(ord);
    world.taskq.add(task_f, ord, idx, trange.make_tile_range(ord), &tile_norms,
                    &tiles[ord].second);
  }
  world.gop.fence();
  auto time_f1 = mpqc::now(world_, true);
  auto time_f = mpqc::duration_in_s(time_f0, time_f1);
  if (print_detail_) {
    utility::print_par(world_, " \tsum of task_f time: ", time_f, " s\n");
  }

  TA::SparseShape<float> shape(world, tile_norms, trange);
  TA::DistArray<Tile, TA::SparsePolicy> out(world, trange, shape, pmap);

  detail::set_array(tiles, out);
  out.truncate();

  return out;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::transform_real2recip(TArray &matrix) {
  TArray result;
  auto tr0 = matrix.trange().data()[0];
  auto tr1 = detail::extend_trange1(tr0, k_size_);

  // Perform real->reciprocal transformation with Eigen
  // TODO: perform it with TA (take arg tile from "matrix",
  // transform it, add it to result tile in "result", construct pmap&shape,
  // use MADNESSworld ...)

  auto matrix_eig = array_ops::array_to_eigen(matrix);
  Matrixz result_eig(tr0.extent(), tr1.extent());
  result_eig.setZero();

  auto threshold = std::numeric_limits<double>::epsilon();
  for (auto R = 0; R < R_size_; ++R) {
    auto bmat =
        matrix_eig.block(0, R * tr0.extent(), tr0.extent(), tr0.extent());
    if (bmat.norm() < bmat.size() * threshold)
      continue;
    else {
      auto vec_R = detail::direct_vector(R, R_max_, dcell_);
      for (auto k = 0; k < k_size_; ++k) {
        auto vec_k = detail::k_vector(k, nk_, dcell_);
        auto exponent = std::exp(I * vec_k.dot(vec_R));
        result_eig.block(0, k * tr0.extent(), tr0.extent(), tr0.extent()) +=
            bmat * exponent;
      }
    }
  }

  result = array_ops::eigen_to_array<Tile>(world_, result_eig, tr0, tr1);

  return result;
}

/// Make PeriodicAOFactory printable
template <typename Tile, typename Policy>
std::ostream &operator<<(std::ostream &os,
                         PeriodicAOFactory<Tile, Policy> &pao) {
  os << "\nPeriodic Hartree-Fock computational parameter:" << std::endl;
  os << "\tR_max (range of expansion of Bloch Gaussians in AO Gaussians): ["
     << pao.R_max().transpose() << "]" << std::endl;
  os << "\tRj_max (range of Coulomb operation): [" << pao.RJ_max().transpose()
     << "]" << std::endl;
  os << "\tRd_max (Range of density representation): ["
     << pao.RD_max().transpose() << "]" << std::endl;
  os << "\t# of k points in each direction: [" << pao.nk().transpose() << "]\n"
     << std::endl;
  return os;
}

}  // namespace integrals
}  // namespace mpqc
#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_PERIODIC_AO_FACTORY_H_