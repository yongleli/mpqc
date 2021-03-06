#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_LCAO_FACTORY_PERIODIC_AO_FACTORY_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_LCAO_FACTORY_PERIODIC_AO_FACTORY_H_

#include "mpqc/chemistry/qc/lcao/factory/ao_factory.h"
#include "mpqc/chemistry/qc/lcao/factory/factory_utility.h"

#include <iosfwd>
#include <vector>

#include "mpqc/chemistry/molecule/lattice/unit_cell.h"
#include "mpqc/chemistry/molecule/lattice/util.h"
#include "mpqc/chemistry/qc/lcao/basis/util.h"
#include "mpqc/chemistry/qc/lcao/integrals/integrals.h"
#include "mpqc/chemistry/units/units.h"
#include "mpqc/math/external/eigen/eigen.h"
#include "mpqc/math/tensor/clr/array_to_eigen.h"
#include "mpqc/util/keyval/keyval.h"
#include "mpqc/util/misc/time.h"

#include <mpqc/chemistry/qc/lcao/integrals/screening/schwarz_screen.h>
#include <unsupported/Eigen/MatrixFunctions>

namespace mpqc {
namespace lcao {
namespace gaussian {

template <typename Tile, typename Policy>
class PeriodicAOFactory;

template <typename Tile, typename Policy>
using PeriodicAOFactoryBase =
    Factory<TA::DistArray<Tile, Policy>, TA::DistArray<Tile, Policy>>;
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
 * \brief Periodic Integral Class
 *
 * This class computes atomic integrals involved in periodic calculations using
 * Formula
 *
 * compute(formula) returns TArray object
 */
template <typename Tile, typename Policy>
class PeriodicAOFactory : public PeriodicAOFactoryBase<Tile, Policy> {
 public:
  using TArray = TA::DistArray<Tile, Policy>;
  using DirectTArray =
      DirectArray<Tile, Policy, DirectIntegralBuilder<Tile, libint2::Engine>>;
  using Op = std::function<Tile(TA::TensorD &&)>;
  using shellpair_list_t = std::vector<std::vector<size_t>>;

 public:
  PeriodicAOFactory() = default;
  PeriodicAOFactory(PeriodicAOFactory &&) = default;
  PeriodicAOFactory &operator=(PeriodicAOFactory &&) = default;

  // clang-format off
  /*!
   * \brief KeyVal constructor for PeriodicAOFactory
   * \param kv The KeyVal object will be queried for all the following keywords:
   *  | Keyword | Type | Default| Description |
   *  |---------|------|--------|-------------|
   *  |\c rmax  | array<int, 3> | none | This gives range of expansion of Bloch Gaussians in AO Gaussians. |
   *  |\c rdmax | array<int, 3> | none | This gives range of Coulomb operation. |
   *  |\c rjmax | array<int, 3> | none | This gives range of density representation. |
   *  |\c engine_precision | real | machine epsilon | This gives integral engine precision. |
   *  |\c screen | string | schwarz | This gives method of screening, qqr or schwarz. |
   *  |\c threshold | real | 1e-20 | This gives threshold for schwarz or qqr screening. |
   *  |\c shell_pair_threshold | real | 1e-12 | This gives threshold for screeing non-negligible shell pairs. |
   *  |\c density_threshold | real | sparse shape threshold | This gives threshold for screening density blocks in Fock build. |
   *  |\c print_detail | bool | false | Print more details if true. |
   *
   *  example input:
   *
   * ~~~~~~~~~~~~~~~~~~~~~{.json}
   *  "wfn_world": {
   *    "atoms" : "$:water",
   *    "basis" : "$:basis",
   *    "df_basis" : "$:dfbs",
   *    "screen": "schwarz",
   *    "threshold": 1.0e-20,
   *    "shell_pair_threshold": 1.0e-20,
   *    "density_threshold": 1.0e-10,
   *    "rmax":  [0, 0, 10],
   *    "rjmax": [0, 0, 10],
   *    "rdmax": [0, 0, 10]
   *  }
   * ~~~~~~~~~~~~~~~~~~~~~
   */
  // clang-format on
  PeriodicAOFactory(const KeyVal &kv)
      : PeriodicAOFactoryBase<Tile, Policy>(kv), kv_(kv) {
    std::string prefix = "";
    if (kv.exists("wfn_world") || kv.exists_class("wfn_world"))
      prefix = "wfn_world:";

    // Molecule was already created at this path, bypass registry and construct
    // UnitCell
    unitcell_ = kv.class_ptr<UnitCell>(prefix + "atoms", true);
    dcell_ = unitcell_->dcell();

    R_max_ =
        decltype(R_max_)(kv.value<std::array<int, 3>>(prefix + "rmax").data());
    RD_max_ = decltype(RD_max_)(
        kv.value<std::array<int, 3>>(prefix + "rdmax").data());
    RJ_max_ = decltype(RJ_max_)(
        kv.value<std::array<int, 3>>(prefix + "rjmax").data());

    using ::mpqc::detail::direct_ord_idx;
    R_size_ = 1 + direct_ord_idx(R_max_, R_max_);
    RJ_size_ = 1 + direct_ord_idx(RJ_max_, RJ_max_);
    RD_size_ = 1 + direct_ord_idx(RD_max_, RD_max_);

    auto default_precision = std::numeric_limits<double>::epsilon();
    engine_precision_ =
        kv.value<double>(prefix + "engine_precision", default_precision);
    detail::integral_engine_precision = engine_precision_;

    screen_ = kv.value<std::string>(prefix + "screen", "schwarz");
    screen_threshold_ = kv.value<double>(prefix + "threshold", 1.0e-20);
    shell_pair_threshold_ =
        kv.value<double>(prefix + "shell_pair_threshold", 1.0e-12);
    density_threshold_ = kv.value<double>(prefix + "density_threshold",
                                          Policy::shape_type::threshold());
    force_hermiticity_ = kv.value<bool>(prefix + "force_hermiticity", true);

    // This functor converts TensorD to TensorZ
    // Uncomment if \tparam Tile = TensorZ
    //    auto convert_op = [](TA::TensorD &&arg) -> TA::TensorZ {
    //      return TA::TensorZ(arg.range(), arg.data());
    //    };
    //    op_ = convert_op;
    detail::set_oper(op_);

    print_detail_ = kv.value<bool>("print_detail", false);

    auto orbital_space_registry =
        std::make_shared<OrbitalSpaceRegistry<TArray>>();

    this->set_orbital_registry(orbital_space_registry);

    // Since R_max_ is only used in |μ_0 ν_R) related ints, it is reasonable to
    // recalculate R_max_ based on the range of significant shell pairs
    renew_overlap_lattice_range();
  }

  ~PeriodicAOFactory() noexcept {}

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
  TArray compute(const Formula &formula) override;

  /// wrapper to compute_direct
  TArray compute_direct(const std::wstring &);

  /// This computes integral direct by Formula
  TArray compute_direct(const Formula &formula) override;

  /// wrapper to compute_direct_vector
  std::vector<DirectTArray> compute_direct_vector(const std::wstring &);

  /// This computes a vector of direct integrals for periodic systems
  std::vector<DirectTArray> compute_direct_vector(const Formula &formula);

  /// wrapper to compute<libint2_oper> function template
  template <libint2::Operator libint2_oper>
  std::array<TArray, libint2::operator_traits<libint2_oper>::nopers> compute(
      const std::wstring &formula_string);

  /*!
   * @brief This computes integrals by \c Formula for those operators that have
   * more than one components, e.g. multipole moments, geometrical derivatives,
   * etc.
   * @tparam libint2_oper libint2 operator type
   * @param formula the desired \c Formula type
   * @return an array of \c TA::DistArray with the array size = \c nopers
   */
  template <libint2::Operator libint2_oper>
  std::array<TArray, libint2::operator_traits<libint2_oper>::nopers> compute(
      const Formula &formula);

  /*!
   * \brief This computes sparse complex array
   *
   * \param world MADNESS world
   * \param engine a utility::TSPool object
   * that is initialized with Operator and bases
   * \param bases std::array of Basis
   * \param p_screen Screener
   * \return the integral sparse array
   */
  TArray compute_integrals(madness::World &world,
                           ShrPool<libint2::Engine> &engine,
                           BasisVector const &bases,
                           std::shared_ptr<Screener> p_screen =
                               std::make_shared<Screener>(Screener{})) {
    auto result = sparse_complex_integrals(world, engine, bases, p_screen, op_);
    return result;
  }

  /*!
   * @brief This computes integrals specified by \c engine and \c bases for
   * those operators that have more than one components, e.g. multipole moments,
   * geometrical derivatives, etc.
   * @tparam libint2_oper libint2 operator type
   * @param world MADNESS World
   * @param engine a \c utility::TSPool object
   * @param bases \c std::array of \c Basis
   * @param p_screen Screener
   * @return \c std::array of \c TA::DistArray
   */
  template <libint2::Operator libint2_oper>
  std::array<TArray, libint2::operator_traits<libint2_oper>::nopers>
  compute_integrals(madness::World &world, ShrPool<libint2::Engine> &engine,
                    BasisVector const &bases,
                    std::shared_ptr<Screener> p_screen =
                        std::make_shared<Screener>(Screener{})) {
    auto result =
        sparse_integrals<libint2_oper>(world, engine, bases, p_screen, op_);
    return result;
  }

  /// This computes sparse complex array using integral direct
  DirectTArray compute_direct_integrals(
      madness::World &world, ShrPool<libint2::Engine> &engine,
      BasisVector const &bases,
      std::shared_ptr<Screener> p_screen =
          std::make_shared<Screener>(Screener{})) {
    auto result =
        direct_sparse_complex_integrals(world, engine, bases, p_screen, op_);
    return result;
  }

 public:
  /// @return a shared ptr to the UnitCell object
  std::shared_ptr<UnitCell> unitcell() const { return unitcell_; }

  /// @return screen method
  const std::string &screen() const { return screen_; }

  /// @return screen threshold
  double screen_threshold() const { return screen_threshold_; }

  /// @return shell pair threshold
  double shell_pair_threshold() const { return shell_pair_threshold_; }

  /// @return density sparsity threshold
  double density_threshold() const { return density_threshold_; }

  /// @return the range of expansion of Bloch Gaussians in AO Gaussians
  Vector3i R_max() { return R_max_; }

  /// @return the range of Coulomb operation
  Vector3i RJ_max() { return RJ_max_; }

  /// @return the range of density representation
  Vector3i RD_max() { return RD_max_; }

  /// @return the cardinal number of lattices included in Bloch Gaussian
  /// expansion
  int64_t R_size() { return R_size_; }

  /// @return the cardinal number of lattices included in Coulomb operation
  int64_t RJ_size() { return RJ_size_; }

  /// @return the cardinal number of lattices included in density representation
  int64_t RD_size() { return RD_size_; }

  /// @return UnitCell object
  UnitCell &unitcell() { return *unitcell_; }

  /// @return boolean of wether to print detailed information
  bool print_detail() { return print_detail_; }

  /*!
   * \brief This sets the density for coulomb and exchange computations
   * \param D the density feeded by user
   */
  void set_density(const TArray &D) { D_ = D; }

  /// @return density matrix
  TArray get_density() { return D_; }

  /// @return significant shell pairs
  const shellpair_list_t &significant_shell_pairs() {
    return sig_shellpair_list_;
  }

  /// @brief This sets the range of Coulomb interaction
  void set_rjmax(const Vector3i &rjmax) {
    RJ_max_ = rjmax;

    using ::mpqc::detail::direct_ord_idx;
    RJ_size_ = 1 + direct_ord_idx(RJ_max_, RJ_max_);
  }

  /// @brief This sets parameters for a libint2 operator
  void set_libint2_operator_params(libint2::any oper_params) {
    libint2_oper_params_ = oper_params;
  }

  /// @brief whether to force hermiticity of Fock matrix
  bool force_hermiticity() { return force_hermiticity_; }

  /// @return integral engine precision
  double engine_precision() { return engine_precision_; }

  /// @return the input keyval
  std::shared_ptr<const KeyVal> keyval() {
    return std::make_shared<const KeyVal>(kv_);
  }

 private:
  /// compute integrals that has two dimensions for periodic systems
  TArray compute2(const Formula &formula);

  /// compute integrals that has two dimensions for periodic systems
  TArray compute3(const Formula &formula);

  /// compute integrals that has four dimensions for periodic systems
  TArray compute4(const Formula &formula);

  /// compute integrals that has three dimensions for periodic systems
  TArray compute_direct3(const Formula &formula);

  /// compute integrals that has four dimensions for periodic systems
  TArray compute_direct4(const Formula &formula);

  /// parse 1-body 1-center formula written as <X|U> and set engine_pool and
  /// basis array
  void parse_one_body_one_center(
      const Formula &formula,
      std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
      BasisVector &bases);

  /// parse 1-body 2-center formula and set engine_pool and basis array for
  /// periodic
  /// system
  void parse_one_body_two_center_periodic(
      const Formula &formula,
      std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
      BasisVector &bases, const Molecule &shifted_mol);

  /// parse 2-body 2-center formula and set engine_pool and basis array for
  /// periodic
  /// system. The ket basis is shifted by \c shift
  void parse_two_body_two_center_periodic(
      const Formula &formula,
      std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
      BasisVector &bases, const Vector3d &shift);

  /// parse 2-body 3-center formula and set engine_pool and basis array for
  /// periodic
  /// system. The ket basis is shifted by \c shift
  void parse_two_body_three_center_periodic(
      const Formula &formula,
      std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
      BasisVector &bases, const Vector3d &shift,
      std::shared_ptr<Screener> &p_screener);

  /// parse 2-body 4-center formula and set engine_pool and basis array for
  /// periodic
  /// system
  void parse_two_body_four_center_periodic(
      const Formula &formula,
      std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
      BasisVector &bases, Vector3d shift_coul, bool if_coulomb,
      std::shared_ptr<Screener> &p_screener);

  /*!
   * \brief Construct sparse complex integral tensors in parallel.
   *
   * \param shr_pool should be a std::shared_ptr to an IntegralTSPool
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
  TArray sparse_complex_integrals(madness::World &world, ShrPool<E> shr_pool,
                                  BasisVector const &bases,
                                  std::shared_ptr<Screener> screen, Op op);

  /// This constructs direct sparse complex integral tensors in parallel
  template <typename E>
  DirectTArray direct_sparse_complex_integrals(madness::World &world,
                                               ShrPool<E> shr_pool,
                                               BasisVector const &bases,
                                               std::shared_ptr<Screener> screen,
                                               Op op);

  /*!
   * @brief This constructs an array of sparse integral \c TA::DistArray in
   * parallel
   * @tparam libint2_oper libint2 operator type
   * @param world MADNESS world
   * @param shr_pool a shared pointer to \c utility::TSPool
   * @param bases \c std::array of \c Basis
   * @param screen a shared pointer to Screener
   * @param op needs to be a function a functor that takes a TA::TensorD && and
   * returns any valid tile type. Op is copied so it can be moved
   * @return \c std::array of \c TA::DistArray
   */
  template <libint2::Operator libint2_oper>
  std::array<TArray, libint2::operator_traits<libint2_oper>::nopers>
  sparse_integrals(madness::World &world, ShrPool<libint2::Engine> shr_pool,
                   BasisVector const &bases, std::shared_ptr<Screener> screen,
                   Op op);

  /// This renews R_max_ and R_size_ based on the range of significant shell
  /// pairs
  void renew_overlap_lattice_range();

 private:
  const KeyVal kv_;
  std::shared_ptr<UnitCell> unitcell_;  ///> UnitCell private member

  Op op_;

  TArray D_;  ///> Density

  Vector3i R_max_ = {
      0, 0, 0};  ///> range of expansion of Bloch Gaussians in AO Gaussians
  Vector3i RJ_max_ = {0, 0, 0};       ///> range of Coulomb operation
  Vector3i RD_max_ = {0, 0, 0};       ///> range of density representation
  Vector3d dcell_ = {0.0, 0.0, 0.0};  ///> direct unit cell params (in a.u.)

  int64_t
      R_size_;  ///> cardinal # of lattices included in Bloch Gaussian expansion
  int64_t RJ_size_;  ///> cardinal # of lattices included in Coulomb operation
  int64_t
      RD_size_;  ///> cardinal # of lattices included in density representation

  bool print_detail_;  ///> if true, print a lot more details
  std::string screen_;
  double engine_precision_;
  double screen_threshold_;
  double shell_pair_threshold_;
  double density_threshold_;
  bool force_hermiticity_;
  std::vector<DirectTArray> gj_;
  std::vector<DirectTArray> gk_;
  std::vector<DirectTArray> g_3idx_;
  shellpair_list_t sig_shellpair_list_;

  libint2::any libint2_oper_params_;
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

  // retrieve the integral if it is in registry
  auto iter = this->registry_.find(formula);

  if (iter != this->registry_.end()) {
    result = iter->second;
    utility::print_par(this->world(), "Retrieved Periodic AO Integral: ",
                       utility::to_string(formula.string()));
    double size = mpqc::detail::array_size(result);
    utility::print_par(this->world(), " Size: ", size, " GB\n");
    return result;
  }

  if (formula.rank() == 2) {
    result = compute2(formula);
    this->registry_.insert(formula, result);
  } else if (formula.rank() == 3) {
    result = compute3(formula);
    this->registry_.insert(formula, result);
  } else if (formula.rank() == 4) {
    result = compute4(formula);
  } else
    throw std::runtime_error("Operator rank not supported");

  return result;
}

template <typename Tile, typename Policy>
template <libint2::Operator libint2_oper>
std::array<typename PeriodicAOFactory<Tile, Policy>::TArray,
           libint2::operator_traits<libint2_oper>::nopers>
PeriodicAOFactory<Tile, Policy>::compute(const std::wstring &formula_string) {
  auto formula = Formula(formula_string);
  return compute<libint2_oper>(formula);
}

template <typename Tile, typename Policy>
template <libint2::Operator libint2_oper>
std::array<typename PeriodicAOFactory<Tile, Policy>::TArray,
           libint2::operator_traits<libint2_oper>::nopers>
PeriodicAOFactory<Tile, Policy>::compute(const Formula &formula) {
  const auto mpqc_oper = formula.oper().type();
  MPQC_ASSERT(detail::to_libint2_operator(mpqc_oper) == libint2_oper);

  const auto nopers = libint2::operator_traits<libint2_oper>::nopers;

  auto &world = this->world();

  ExEnv::out0() << "\nComputing Two Center Integral for Periodic System: "
                << utility::to_string(formula.string()) << std::endl;

  auto time0 = mpqc::fenced_now(world);

  std::array<TArray, libint2::operator_traits<libint2_oper>::nopers> result;
  if (formula.rank() == 2) {
    if (mpqc_oper == Operator::Type::SphericalMultipole) {
      auto bra_index = formula.bra_indices()[0];
      auto ket_index = formula.ket_indices()[0];
      auto bra_basis = this->basis_registry()->retrieve(bra_index);
      auto ket_basis = this->basis_registry()->retrieve(ket_index);

      // Form a compound ket basis by shifting origins from -Rmax to Rmax
      ket_basis = detail::shift_basis_origin(*ket_basis, Vector3d::Zero(),
                                             R_max_, dcell_);
      auto bases = BasisVector{{*bra_basis, *ket_basis}};
      auto engine_pool = make_engine_pool(
          libint2_oper, utility::make_array_of_refs(*bra_basis, *ket_basis),
          libint2::BraKet::x_x, libint2_oper_params_);

      result = compute_integrals<libint2_oper>(world, engine_pool, bases);
    }
  } else {
    throw FeatureNotImplemented("Operator rank != 2 not supported");
  }

  auto time1 = mpqc::fenced_now(world);
  auto time = mpqc::duration_in_s(time0, time1);

  double size = 0.0;
  for (auto i = 0u; i != nopers; ++i) {
    size += mpqc::detail::array_size(result[i]);
  }
  utility::print_par(this->world(), " Size: ", size, " GB");
  utility::print_par(this->world(), " Time: ", time, " s\n");

  return result;
};

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute2(const Formula &formula) {
  BasisVector bs_array;
  TArray result;
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  double size = 0.0;

  ExEnv::out0() << "\nComputing Two Center Integral for Periodic System: "
                << utility::to_string(formula.string()) << std::endl;

  auto time0 = mpqc::now(world, this->accurate_time());

  if (formula.oper().type() == Operator::Type::Identity) {
    // Identity matrix

    auto bra_index = formula.bra_indices()[0];
    auto ket_index = formula.ket_indices()[0];
    auto bra_basis = this->basis_registry()->retrieve(bra_index);
    auto ket_basis = this->basis_registry()->retrieve(ket_index);
    auto bra_tr = bra_basis->create_trange1();
    auto ket_tr = ket_basis->create_trange1();
    // create diagonal array
    result = math::create_diagonal_array_from_eigen<Tile, Policy>(
        world, bra_tr, ket_tr, 1.0);
    result.truncate();

  } else if (formula.bra_indices()[0] == OrbitalIndex(L"U") ||
             formula.ket_indices()[0] == OrbitalIndex(L"U")) {
    // compute one center integrals written as <X|U> or <U|X>
    parse_one_body_one_center(formula, engine_pool, bs_array);
    auto result_2D = compute_integrals(world, engine_pool, bs_array);
    result = result_2D;

  } else if (formula.oper().type() == Operator::Type::Kinetic ||
             formula.oper().type() == Operator::Type::Overlap) {
    parse_one_body_two_center_periodic(formula, engine_pool, bs_array,
                                       *unitcell_);
    result = compute_integrals(world, engine_pool, bs_array);
  } else if (formula.oper().type() == Operator::Type::Nuclear) {
    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      using ::mpqc::detail::shift_mol_origin;
      auto shift_mol = direct_vector(RJ, RJ_max_, dcell_);
      auto shifted_mol = shift_mol_origin(*unitcell_, shift_mol);
      parse_one_body_two_center_periodic(formula, engine_pool, bs_array,
                                         *shifted_mol);
      if (RJ == 0)
        result = compute_integrals(world, engine_pool, bs_array);
      else
        result("mu, nu") +=
            compute_integrals(world, engine_pool, bs_array)("mu, nu");
    }
  } else if (formula.oper().type() == Operator::Type::Coulomb) {
    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
      parse_two_body_two_center_periodic(formula, engine_pool, bs_array,
                                         vec_RJ);
      if (RJ == 0)
        result = compute_integrals(world, engine_pool, bs_array);
      else
        result("mu, nu") +=
            compute_integrals(world, engine_pool, bs_array)("mu, nu");
    }
  } else
    throw std::runtime_error("Rank-2 operator type not supported");
  auto time1 = mpqc::now(world, this->accurate_time());
  auto time = mpqc::duration_in_s(time0, time1);

  size = mpqc::detail::array_size(result);
  utility::print_par(world, " Size: ", size, " GB");
  utility::print_par(world, " Time: ", time, " s\n");

  return result;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute3(const Formula &formula) {
  TArray result;
  BasisVector bs_array;
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  std::shared_ptr<Screener> p_screener = std::make_shared<Screener>(Screener{});

  double size = 0.0;

  ExEnv::out0() << "\nComputing Three Center Integral for Periodic System: "
                << utility::to_string(formula.string()) << std::endl;

  auto time0 = mpqc::now(world, this->accurate_time());
  if (formula.oper().type() == Operator::Type::Coulomb) {
    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;

      auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
      parse_two_body_three_center_periodic(formula, engine_pool, bs_array,
                                           vec_RJ, p_screener);
      auto g = compute_integrals(world, engine_pool, bs_array, p_screener);

      if (RJ == 0)
        result("K, mu, nu") = g("K, mu, nu");
      else
        result("K, mu, nu") += g("K, mu, nu");
    }
  } else
    throw std::runtime_error("Rank-3 operator type not supported");
  auto time1 = mpqc::now(world, this->accurate_time());
  auto time = mpqc::duration_in_s(time0, time1);

  size = mpqc::detail::array_size(result);
  utility::print_par(world, " Size: ", size, " GB");
  utility::print_par(world, " Time: ", time, " s\n");

  return result;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute4(const Formula &formula) {
  BasisVector bs_array;
  TArray result;
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  double size = 0.0;

  auto time_4idx = 0.0;
  auto time_contr = 0.0;
  auto time = 0.0;

  std::shared_ptr<Screener> p_screener = std::make_shared<Screener>(Screener{});

  if (print_detail_) {
    utility::print_par(world,
                       "\nComputing Two Body Integral for Periodic System: ",
                       utility::to_string(formula.string()), "\n");
  }

  if (formula.oper().type() == Operator::Type::J) {
    auto time_j0 = mpqc::now(world, this->accurate_time());

    auto j_formula = formula;
    j_formula.set_operator_type(Operator::Type::Coulomb);

    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
      parse_two_body_four_center_periodic(j_formula, engine_pool, bs_array,
                                          vec_RJ, true, p_screener);

      auto time_g0 = mpqc::now(world, this->accurate_time());
      auto g = compute_integrals(world, engine_pool, bs_array, p_screener);
      auto time_g1 = mpqc::now(world, this->accurate_time());

      time_4idx += mpqc::duration_in_s(time_g0, time_g1);

      auto time_contr0 = mpqc::now(world, this->accurate_time());
      if (RJ == 0)
        result("mu, nu") = g("mu, nu, lambda, rho") * D_("lambda, rho");
      else
        result("mu, nu") += g("mu, nu, lambda, rho") * D_("lambda, rho");
      auto time_contr1 = mpqc::now(world, this->accurate_time());
      time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
    }
    auto time_j1 = mpqc::now(world, this->accurate_time());
    time = mpqc::duration_in_s(time_j0, time_j1);

  } else if (formula.oper().type() == Operator::Type::K) {
    auto time_k0 = mpqc::now(world, this->accurate_time());

    auto k_formula = formula;
    k_formula.set_operator_type(Operator::Type::Coulomb);
    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
      parse_two_body_four_center_periodic(k_formula, engine_pool, bs_array,
                                          vec_RJ, false, p_screener);

      auto time_g0 = mpqc::now(world, this->accurate_time());
      auto g = compute_integrals(world, engine_pool, bs_array, p_screener);
      auto time_g1 = mpqc::now(world, this->accurate_time());

      time_4idx += mpqc::duration_in_s(time_g0, time_g1);

      auto time_contr0 = mpqc::now(world, this->accurate_time());
      if (RJ == 0)
        result("mu, nu") = g("mu, lambda, nu, rho") * D_("lambda, rho");
      else
        result("mu, nu") += g("mu, lambda, nu, rho") * D_("lambda, rho");
      auto time_contr1 = mpqc::now(world, this->accurate_time());
      time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
    }
    auto time_k1 = mpqc::now(world, this->accurate_time());
    time = mpqc::duration_in_s(time_k0, time_k1);

  } else
    throw std::runtime_error("Rank-4 operator type not supported");

  if (print_detail_) {
    size = mpqc::detail::array_size(result);
    utility::print_par(world, " Size: ", size, " GB\n");
    utility::print_par(world, " \t4-index g tensor time: ", time_4idx, " s\n");
    utility::print_par(world, " \tg*D contraction time: ", time_contr, " s\n");
    utility::print_par(world, " \ttotal time: ", time, " s\n");
  }

  return result;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute_direct(
    const std::wstring &formula_string) {
  auto formula = Formula(formula_string);
  return compute_direct(formula);
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute_direct(const Formula &formula) {
  TA_USER_ASSERT(lcao::detail::if_all_ao(formula),
                 "AOFactory only accept AO index!\n");
  TArray result;
  // retrieve the integral if it is in registry
  auto iter = this->registry_.find(formula);

  if (iter != this->registry_.end()) {
    result = iter->second;
    utility::print_par(this->world(), "Retrieved Periodic AO Integral: ",
                       utility::to_string(formula.string()));
    double size = mpqc::detail::array_size(result);
    utility::print_par(this->world(), " Size: ", size, " GB\n");
    return result;
  } else {
    if (formula.rank() == 2) {
      result = compute2(formula);
      this->registry_.insert(formula, result);
    } else if (formula.rank() == 3) {
      result = compute_direct3(formula);
    } else if (formula.rank() == 4) {
      result = compute_direct4(formula);
    } else
      throw std::runtime_error("Operator rank not supported");
  }

  return result;
}

template <typename Tile, typename Policy>
std::vector<typename PeriodicAOFactory<Tile, Policy>::DirectTArray>
PeriodicAOFactory<Tile, Policy>::compute_direct_vector(
    const std::wstring &formula_string) {
  auto formula = Formula(formula_string);
  return compute_direct_vector(formula);
}

template <typename Tile, typename Policy>
std::vector<typename PeriodicAOFactory<Tile, Policy>::DirectTArray>
PeriodicAOFactory<Tile, Policy>::compute_direct_vector(const Formula &formula) {
  BasisVector bs_array;
  auto result = std::vector<DirectTArray>(RJ_size_, DirectTArray());
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  std::shared_ptr<Screener> p_screener = std::make_shared<Screener>(Screener{});

  double size = 0.0;
  auto time0 = mpqc::now(world, this->accurate_time());

  if (formula.rank() == 3) {
    ExEnv::out0() << "\nComputing Vector of Three Center Direct Integral for "
                     "Periodic System: "
                  << utility::to_string(formula.string()) << std::endl;
    if (formula.oper().type() == Operator::Type::Coulomb) {
      for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
        using ::mpqc::detail::direct_vector;

        DirectTArray &g = result[RJ];
        if (!g.array().is_initialized()) {
          auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
          parse_two_body_three_center_periodic(formula, engine_pool, bs_array,
                                               vec_RJ, p_screener);
          g = compute_direct_integrals(world, engine_pool, bs_array,
                                       p_screener);
          size += mpqc::detail::array_size(g.array());
        }
      }
    } else
      throw std::runtime_error("Rank-3 operator type not supported");

  } else
    throw std::runtime_error("Operator rank not supported");

  auto time1 = mpqc::now(world, this->accurate_time());
  auto time = mpqc::duration_in_s(time0, time1);

  utility::print_par(world, " Size: ", size, " GB");
  utility::print_par(world, " Time: ", time, " s\n");

  return result;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute_direct3(const Formula &formula) {
  TArray result;
  BasisVector bs_array;
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  std::shared_ptr<Screener> p_screener = std::make_shared<Screener>(Screener{});

  double size = 0.0;
  auto time_3idx = 0.0;
  auto time_contr = 0.0;

  if (print_detail_) {
    utility::print_par(world,
                       "\nComputing Three Body Integral for Periodic System: ",
                       utility::to_string(formula.string()), "\n");
  }

  auto time0 = mpqc::now(world, this->accurate_time());
  if (formula.oper().type() == Operator::Type::Coulomb) {
    if (g_3idx_.empty()) {
      g_3idx_ = std::vector<DirectTArray>(RJ_size_, DirectTArray());
    }

    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      DirectTArray &g = g_3idx_[RJ];

      if (!g.array().is_initialized()) {
        auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
        parse_two_body_three_center_periodic(formula, engine_pool, bs_array,
                                             vec_RJ, p_screener);
        auto time_g0 = mpqc::now(world, this->accurate_time());
        g = compute_direct_integrals(world, engine_pool, bs_array, p_screener);
        auto time_g1 = mpqc::now(world, this->accurate_time());

        time_3idx += mpqc::duration_in_s(time_g0, time_g1);
      }

      auto time_contr0 = mpqc::now(world, this->accurate_time());
      if (RJ == 0)
        result("K") = g("K, mu, nu") * D_("mu, nu");
      else
        result("K") += g("K, mu, nu") * D_("mu, nu");
      auto time_contr1 = mpqc::now(world, this->accurate_time());
      time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
    }
  } else
    throw std::runtime_error("Rank-3 operator type not supported");
  auto time1 = mpqc::now(world, this->accurate_time());
  auto time = mpqc::duration_in_s(time0, time1);

  if (print_detail_) {
    size = mpqc::detail::array_size(result);
    utility::print_par(world, " Size: ", size, " GB\n");
    utility::print_par(world, " \t3-index g tensor time: ", time_3idx, " s\n");
    utility::print_par(world, " \tg*D contraction time: ", time_contr, " s\n");
    utility::print_par(world, " \ttotal time: ", time, " s\n");
  }

  return result;
}

template <typename Tile, typename Policy>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::compute_direct4(const Formula &formula) {
  BasisVector bs_array;
  TArray result;
  std::shared_ptr<utility::TSPool<libint2::Engine>> engine_pool;
  auto &world = this->world();
  double size = 0.0;

  auto time_4idx = 0.0;
  auto time_contr = 0.0;
  auto time = 0.0;

  std::shared_ptr<Screener> p_screener = std::make_shared<Screener>(Screener{});

  if (print_detail_) {
    utility::print_par(world,
                       "\nComputing Two Body Integral for Periodic System: ",
                       utility::to_string(formula.string()), "\n");
  }

  if (formula.oper().type() == Operator::Type::J) {
    auto time_j0 = mpqc::now(world, this->accurate_time());

    auto j_formula = formula;
    j_formula.set_operator_type(Operator::Type::Coulomb);

    if (gj_.empty()) {
      gj_ = std::vector<DirectTArray>(RJ_size_, DirectTArray());
    }

    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;

      DirectTArray &g = gj_[RJ];
      if (!g.array().is_initialized()) {
        auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
        parse_two_body_four_center_periodic(j_formula, engine_pool, bs_array,
                                            vec_RJ, true, p_screener);

        auto time_g0 = mpqc::now(world, this->accurate_time());
        g = compute_direct_integrals(world, engine_pool, bs_array, p_screener);
        auto time_g1 = mpqc::now(world, this->accurate_time());

        time_4idx += mpqc::duration_in_s(time_g0, time_g1);
      }

      auto time_contr0 = mpqc::now(world, this->accurate_time());
      if (RJ == 0)
        result("mu, nu") = g("mu, nu, lambda, rho") * D_("lambda, rho");
      else
        result("mu, nu") += g("mu, nu, lambda, rho") * D_("lambda, rho");
      auto time_contr1 = mpqc::now(world, this->accurate_time());
      time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
    }
    auto time_j1 = mpqc::now(world, this->accurate_time());
    time = mpqc::duration_in_s(time_j0, time_j1);

  } else if (formula.oper().type() == Operator::Type::K) {
    auto time_k0 = mpqc::now(world, this->accurate_time());

    auto k_formula = formula;
    k_formula.set_operator_type(Operator::Type::Coulomb);

    if (gk_.empty()) {
      gk_ = std::vector<DirectTArray>(RJ_size_, DirectTArray());
    }

    for (auto RJ = 0; RJ < RJ_size_; ++RJ) {
      using ::mpqc::detail::direct_vector;
      DirectTArray &g = gk_[RJ];
      if (!g.array().is_initialized()) {
        auto vec_RJ = direct_vector(RJ, RJ_max_, dcell_);
        parse_two_body_four_center_periodic(k_formula, engine_pool, bs_array,
                                            vec_RJ, false, p_screener);

        auto time_g0 = mpqc::now(world, this->accurate_time());
        g = compute_direct_integrals(world, engine_pool, bs_array, p_screener);
        auto time_g1 = mpqc::now(world, this->accurate_time());

        time_4idx += mpqc::duration_in_s(time_g0, time_g1);
      }

      auto time_contr0 = mpqc::now(world, this->accurate_time());
      if (RJ == 0)
        result("mu, nu") = g("mu, lambda, nu, rho") * D_("lambda, rho");
      else
        result("mu, nu") += g("mu, lambda, nu, rho") * D_("lambda, rho");
      auto time_contr1 = mpqc::now(world, this->accurate_time());
      time_contr += mpqc::duration_in_s(time_contr0, time_contr1);
    }
    auto time_k1 = mpqc::now(world, this->accurate_time());
    time = mpqc::duration_in_s(time_k0, time_k1);

  } else
    throw ProgrammingError("Rank-4 operator type not supported", __FILE__,
                           __LINE__);

  if (print_detail_) {
    size = mpqc::detail::array_size(result);
    utility::print_par(world, " Size: ", size, " GB\n");
    utility::print_par(world, " \t4-index g tensor time: ", time_4idx, " s\n");
    utility::print_par(world, " \tg*D contraction time: ", time_contr, " s\n");
    utility::print_par(world, " \ttotal time: ", time, " s\n");
  }

  return result;
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_one_body_one_center(
    const Formula &formula,
    std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
    BasisVector &bases) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 1);
  TA_ASSERT(ket_indices.size() == 1);

  auto bra_index = bra_indices[0];
  auto ket_index = ket_indices[0];

  TA_ASSERT(bra_index == OrbitalIndex(L"U") || ket_index == OrbitalIndex(L"U"));

  if (bra_index == OrbitalIndex(L"U")) {
    TA_ASSERT(ket_index.is_ao());
  } else {
    TA_ASSERT(bra_index.is_ao());
  }

  const auto &basis_registry = *this->basis_registry();

  auto bra_basis = detail::index_to_basis(basis_registry, bra_index);
  auto ket_basis = detail::index_to_basis(basis_registry, ket_index);

  TA_ASSERT(bra_basis != nullptr);
  TA_ASSERT(ket_basis != nullptr);

  bases = BasisVector{{*bra_basis, *ket_basis}};

  auto oper_type = formula.oper().type();
  engine_pool = make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(*bra_basis, *ket_basis), libint2::BraKet::x_x,
      detail::to_libint2_operator_params(oper_type, *unitcell_));
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_one_body_two_center_periodic(
    const Formula &formula,
    std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
    BasisVector &bases, const Molecule &shifted_mol) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 1);
  TA_ASSERT(ket_indices.size() == 1);

  auto bra_index = bra_indices[0];
  auto ket_index = ket_indices[0];

  TA_ASSERT(bra_index.is_ao());
  TA_ASSERT(ket_index.is_ao());

  const auto &basis_registry = *this->basis_registry();

  auto bra_basis = detail::index_to_basis(basis_registry, bra_index);
  auto ket_basis = detail::index_to_basis(basis_registry, ket_index);

  TA_ASSERT(bra_basis != nullptr);
  TA_ASSERT(ket_basis != nullptr);

  // Form a compound ket basis by shifting origins from -Rmax to Rmax
  ket_basis =
      detail::shift_basis_origin(*ket_basis, Vector3d::Zero(), R_max_, dcell_);

  bases = BasisVector{{*bra_basis, *ket_basis}};

  auto oper_type = formula.oper().type();
  engine_pool = make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(*bra_basis, *ket_basis), libint2::BraKet::x_x,
      detail::to_libint2_operator_params(oper_type, shifted_mol));
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_two_body_two_center_periodic(
    const Formula &formula,
    std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
    BasisVector &bases, const Vector3d &shift) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 1);
  TA_ASSERT(ket_indices.size() == 1);

  auto bra_index = bra_indices[0];
  auto ket_index = ket_indices[0];

  TA_ASSERT(bra_index.is_ao());
  TA_ASSERT(ket_index.is_ao());

  const auto &basis_registry = *this->basis_registry();

  auto bra_basis = detail::index_to_basis(basis_registry, bra_index);
  auto ket_basis = detail::index_to_basis(basis_registry, ket_index);

  TA_ASSERT(bra_basis != nullptr);
  TA_ASSERT(ket_basis != nullptr);

  // Shift ket basis
  ket_basis = detail::shift_basis_origin(*ket_basis, shift);

  bases = BasisVector{{*bra_basis, *ket_basis}};

  auto oper_type = formula.oper().type();
  engine_pool = make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(*bra_basis, *ket_basis),
      libint2::BraKet::xs_xs,
      detail::to_libint2_operator_params(oper_type, *unitcell_));
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_two_body_three_center_periodic(
    const Formula &formula,
    std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
    BasisVector &bases, const Vector3d &shift,
    std::shared_ptr<Screener> &p_screener) {
  auto bra_indices = formula.bra_indices();
  auto ket_indices = formula.ket_indices();

  TA_ASSERT(bra_indices.size() == 1);
  TA_ASSERT(ket_indices.size() == 2);

  auto bra_index0 = bra_indices[0];
  auto ket_index0 = ket_indices[0];
  auto ket_index1 = ket_indices[1];

  TA_ASSERT(bra_index0.is_ao());
  TA_ASSERT(ket_index0.is_ao());
  TA_ASSERT(ket_index1.is_ao());

  const auto &basis_registry = *this->basis_registry();

  auto bra_basis0 = detail::index_to_basis(basis_registry, bra_index0);
  auto ket_basis0 = detail::index_to_basis(basis_registry, ket_index0);
  auto ket_basis1 = detail::index_to_basis(basis_registry, ket_index1);

  TA_ASSERT(bra_basis0 != nullptr);
  TA_ASSERT(ket_basis0 != nullptr);
  TA_ASSERT(ket_basis1 != nullptr);

  // Form a compound index ket1 basis
  ket_basis1 =
      detail::shift_basis_origin(*ket_basis1, Vector3d::Zero(), R_max_, dcell_);
  // Shift bra basis
  bra_basis0 = detail::shift_basis_origin(*bra_basis0, shift);

  if (formula.notation() == Formula::Notation::Chemical)
    bases = BasisVector{{*bra_basis0, *ket_basis0, *ket_basis1}};
  else
    throw "Physical notation not supported!";

  auto oper_type = formula.oper().type();
  engine_pool = make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(*bra_basis0, *ket_basis0, *ket_basis1),
      libint2::BraKet::xs_xx,
      detail::to_libint2_operator_params(oper_type, *unitcell_));

  if (!screen_.empty()) {
    auto screen_engine_pool = make_engine_pool(
        detail::to_libint2_operator(oper_type),
        utility::make_array_of_refs(*bra_basis0, *ket_basis0, *ket_basis1),
        libint2::BraKet::xx_xx,
        detail::to_libint2_operator_params(oper_type, *unitcell_));

    p_screener =
        detail::make_screener(this->world(), screen_engine_pool, bases,
                              this->screen(), this->screen_threshold());
  }
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::parse_two_body_four_center_periodic(
    const Formula &formula,
    std::shared_ptr<utility::TSPool<libint2::Engine>> &engine_pool,
    BasisVector &bases, Vector3d shift_coul, bool if_coulomb,
    std::shared_ptr<Screener> &p_screener) {
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

  const auto &basis_registry = *this->basis_registry();

  auto bra_basis0 = detail::index_to_basis(basis_registry, bra_index0);
  auto bra_basis1 = detail::index_to_basis(basis_registry, bra_index1);
  auto ket_basis0 = detail::index_to_basis(basis_registry, ket_index0);
  auto ket_basis1 = detail::index_to_basis(basis_registry, ket_index1);

  TA_ASSERT(bra_basis0 != nullptr);
  TA_ASSERT(bra_basis1 != nullptr);
  TA_ASSERT(ket_basis0 != nullptr);
  TA_ASSERT(ket_basis1 != nullptr);

  // Form a compound index basis
  if (if_coulomb) {
    bra_basis1 = detail::shift_basis_origin(*bra_basis1, Vector3d::Zero(),
                                            R_max_, dcell_);
    ket_basis0 = detail::shift_basis_origin(*ket_basis0, shift_coul);
  } else {
    bra_basis1 = detail::shift_basis_origin(*bra_basis1, shift_coul);
    ket_basis0 = detail::shift_basis_origin(*ket_basis0, Vector3d::Zero(),
                                            R_max_, dcell_);
  }
  ket_basis1 =
      detail::shift_basis_origin(*ket_basis1, shift_coul, RD_max_, dcell_);

  if (formula.notation() == Formula::Notation::Chemical)
    bases = BasisVector{{*bra_basis0, *bra_basis1, *ket_basis0, *ket_basis1}};
  else
    throw "Physical notation not supported!";

  auto oper_type = formula.oper().type();
  engine_pool = make_engine_pool(
      detail::to_libint2_operator(oper_type),
      utility::make_array_of_refs(bases[0], bases[1], bases[2], bases[3]),
      libint2::BraKet::xx_xx,
      detail::to_libint2_operator_params(oper_type, *unitcell_));

  p_screener = detail::make_screener(this->world(), engine_pool, bases,
                                     this->screen(), this->screen_threshold());
}

template <typename Tile, typename Policy>
template <typename E>
typename PeriodicAOFactory<Tile, Policy>::TArray
PeriodicAOFactory<Tile, Policy>::sparse_complex_integrals(
    madness::World &world, ShrPool<E> shr_pool, BasisVector const &bases,
    std::shared_ptr<Screener> screen, Op op) {
  // Build the Trange and Shape Tensor
  auto trange = detail::create_trange(bases);
  const auto tvolume = trange.tiles_range().volume();
  std::vector<std::pair<unsigned long, Tile>> tiles(tvolume);
  TA::TensorF tile_norms(trange.tiles_range(), 0.0);

  // Copy the Bases for the Integral Builder
  auto shr_bases = std::make_shared<BasisVector>(bases);

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

  for (auto const ord : *pmap) {
    tiles[ord].first = ord;
    detail::IdxVec idx = trange.tiles_range().idx(ord);
    world.taskq.add(task_f, ord, idx, trange.make_tile_range(ord), &tile_norms,
                    &tiles[ord].second);
  }
  world.gop.fence();

  TA::SparseShape<float> shape(world, tile_norms, trange);
  TA::DistArray<Tile, TA::SparsePolicy> out(world, trange, shape, pmap);

  detail::set_array(tiles, out);
  world.gop.fence();
  out.truncate();
  world.gop.fence();

  return out;
}

template <typename Tile, typename Policy>
template <libint2::Operator libint2_oper>
std::array<typename PeriodicAOFactory<Tile, Policy>::TArray,
           libint2::operator_traits<libint2_oper>::nopers>
PeriodicAOFactory<Tile, Policy>::sparse_integrals(
    madness::World &world, ShrPool<libint2::Engine> shr_pool,
    BasisVector const &bases, std::shared_ptr<Screener> screen, Op op) {
  const auto nopers = libint2::operator_traits<libint2_oper>::nopers;

  // build the trange and shape tensor
  auto trange = detail::create_trange(bases);
  const auto tvolume = trange.tiles_range().volume();

  using TileVec = std::vector<std::pair<unsigned long, Tile>>;
  using NormArray =
      std::array<TA::TensorF, libint2::operator_traits<libint2_oper>::nopers>;
  using TilePtrArray =
      std::array<Tile *, libint2::operator_traits<libint2_oper>::nopers>;

  TileVec tiles(tvolume);

  NormArray array_of_tile_norms;
  for (auto &norms : array_of_tile_norms) {
    norms = TA::TensorF(trange.tiles_range(), 0.0);
  }

  // Copy the Bases for the Integral Builder
  auto shr_bases = std::make_shared<BasisVector>(bases);

  // Make a pointer to an Integral builder.  Doing this because we want to use
  // it in Tasks.
  auto builder_ptr = std::make_shared<IntegralBuilder<Tile, libint2::Engine>>(
      std::move(shr_pool), std::move(shr_bases), std::move(screen),
      std::move(op));

  auto task_f = [=](int64_t ord, detail::IdxVec idx, TA::Range rng,
                    NormArray *norms_array_ptr,
                    TilePtrArray *tileptr_array_ptr) {
    // This is why builder was made into a shared_ptr.
    auto &builder = *builder_ptr;
    auto &norms_array = *norms_array_ptr;
    auto &tileptr_array = *tileptr_array_ptr;

    auto ta_tile_array =
        builder.template integrals<libint2_oper>(idx, std::move(rng));

    for (auto oper = 0; oper != nopers; ++oper) {
      const auto tile_volume = ta_tile_array[oper].range().volume();
      const auto tile_norm = ta_tile_array[oper].norm();

      // Keep tile if it was significant.
      bool save_norm =
          tile_norm >= tile_volume * TA::SparseShape<float>::threshold();
      if (save_norm) {
        *(tileptr_array[oper]) = builder.op(std::move(ta_tile_array[oper]));
        norms_array[oper][ord] = tile_norm;
      }
    }
  };

  auto pmap = TA::SparsePolicy::default_pmap(world, tvolume);

  using TileVecArray =
      std::array<TileVec, libint2::operator_traits<libint2_oper>::nopers>;
  TileVecArray array_of_tilevec;
  array_of_tilevec.fill(tiles);

  std::vector<TilePtrArray> vector_of_array_of_tileptr(tvolume);
  for (auto const ord : *pmap) {
    for (auto oper = 0u; oper != nopers; ++oper) {
      array_of_tilevec[oper][ord].first = ord;
      vector_of_array_of_tileptr[ord][oper] =
          &(array_of_tilevec[oper][ord].second);
    }
    detail::IdxVec idx = trange.tiles_range().idx(ord);
    world.taskq.add(task_f, ord, idx, trange.make_tile_range(ord),
                    &array_of_tile_norms, &(vector_of_array_of_tileptr[ord]));
  }
  world.gop.fence();

  using TArrayArray =
      std::array<TArray, libint2::operator_traits<libint2_oper>::nopers>;
  TArrayArray result;

  for (auto oper = 0u; oper != nopers; ++oper) {
    auto &norms = array_of_tile_norms[oper];
    TA::SparseShape<float> shape(world, norms, trange);
    TArray &tarray = result[oper];
    tarray = TArray(world, trange, shape, pmap);
    if (norms.norm() >=
        norms.range().volume() * TA::SparseShape<float>::threshold()) {
      detail::set_array(array_of_tilevec[oper], tarray);
    }
  }
  world.gop.fence();

  for (auto oper = 0u; oper != nopers; ++oper) {
    result[oper].truncate();
  }
  world.gop.fence();

  return result;
}

template <typename Tile, typename Policy>
template <typename E>
typename PeriodicAOFactory<Tile, Policy>::DirectTArray
PeriodicAOFactory<Tile, Policy>::direct_sparse_complex_integrals(
    madness::World &world, ShrPool<E> shr_pool, BasisVector const &bases,
    std::shared_ptr<Screener> screen, Op op) {
  // Build the Trange and Shape Tensor
  auto trange = detail::create_trange(bases);

  auto pmap = Policy::default_pmap(world, trange.tiles_range().volume());

  TA::TensorF tile_norms = screen->norm_estimate(world, bases, *pmap);

  TA::SparseShape<float> shape(world, tile_norms, trange);

  // Copy the Bases for the Integral Builder
  auto shr_bases = std::make_shared<BasisVector>(bases);

  // Make a pointer to an Integral builder.  Doing this because we want to use
  // it in Tasks.
  auto builder = make_direct_integral_builder(world, std::move(shr_pool),
                                              std::move(shr_bases),
                                              std::move(screen), std::move(op));

  auto direct_array = DirectArray<Tile, Policy, DirectIntegralBuilder<Tile, E>>(
      std::move(builder));
  auto builder_ptr = direct_array.builder();
  using DirectTileType = DirectTile<Tile, DirectIntegralBuilder<Tile, E>>;

  auto task_f = [=](int64_t ord, detail::IdxVec idx, TA::Range rng) {

    return DirectTileType(idx, std::move(rng), std::move(builder_ptr));

  };

  TA::DistArray<DirectTileType, TA::SparsePolicy> out(world, trange,
                                                      std::move(shape), pmap);

  for (auto const &ord : *pmap) {
    if (!out.is_zero(ord)) {
      detail::IdxVec idx = trange.tiles_range().idx(ord);
      auto range = trange.make_tile_range(ord);
      auto tile_fut = world.taskq.add(task_f, ord, idx, range);
      out.set(ord, tile_fut);
    }
  }
  world.gop.fence();

  direct_array.set_array(std::move(out));
  return direct_array;
}

/// Make PeriodicAOFactory printable
template <typename Tile, typename Policy>
std::ostream &operator<<(std::ostream &os,
                         PeriodicAOFactory<Tile, Policy> &pao) {
  os << "\nPeriodicAOFactory computational parameters:" << std::endl;
  os << "\tR_max (range of expansion of Bloch Gaussians in AO Gaussians): ["
     << pao.R_max().transpose() << "]" << std::endl;
  os << "\tRj_max (range of Coulomb operation): [" << pao.RJ_max().transpose()
     << "]" << std::endl;
  os << "\tRd_max (Range of density representation): ["
     << pao.RD_max().transpose() << "]" << std::endl;
  os << "\tEngine precision = " << pao.engine_precision() << std::endl;
  os << "\tNon-negligible shell-pair threshold = " << pao.shell_pair_threshold()
     << std::endl;
  os << "\tDensity threshold = " << pao.density_threshold() << std::endl;
  os << "\tScreen threshold = " << pao.screen_threshold() << std::endl;

  auto hermiticity = pao.force_hermiticity() ? "True" : "False";
  os << "\tForce Fock hermiticity: " << hermiticity << std::endl;

  return os;
}

template <typename Tile, typename Policy>
void PeriodicAOFactory<Tile, Policy>::renew_overlap_lattice_range() {
  using ::mpqc::detail::direct_3D_idx;
  using ::mpqc::detail::direct_ord_idx;
  using ::mpqc::lcao::gaussian::detail::parallel_compute_shellpair_list;
  using ::mpqc::lcao::gaussian::detail::shift_basis_origin;

  ExEnv::out0() << "\nUser specified range of lattice sum for |mu nu_R) = "
                << R_max_.transpose() << std::endl;

  // compute significant shell pair list
  auto basis_ptr = this->basis_registry()->retrieve(OrbitalIndex(L"λ"));
  auto basisR_ptr =
      shift_basis_origin(*basis_ptr, Vector3d::Zero(), R_max_, dcell_);
  sig_shellpair_list_ =
      parallel_compute_shellpair_list(this->world(), *basis_ptr, *basisR_ptr,
                                      shell_pair_threshold_, engine_precision_);

  // make a list of significant R's as in overlap between μ_0 and ν_R
  std::vector<Vector3i> sig_lattice_list;
  const auto nshells_per_uc = basis_ptr->flattened_shells().size();
  for (auto R_ord = 0; R_ord != R_size_; ++R_ord) {
    const auto R_3D = direct_3D_idx(R_ord, R_max_);
    const auto shell1_min = nshells_per_uc * R_ord;
    const auto shell1_max = shell1_min + nshells_per_uc;

    auto is_significant = false;
    for (auto shell0 = 0ul; shell0 != nshells_per_uc; ++shell0) {
      for (const auto &shell1 : sig_shellpair_list_[shell0]) {
        if (shell1 >= shell1_min && shell1 < shell1_max) {
          is_significant = true;
          sig_lattice_list.emplace_back(R_3D);
          break;
        }
      }
      if (is_significant) break;
    }
  }

  // renew the range of lattice sum for |μ_0 ν_Rν) based on the list of
  // significant lattice vectors
  auto x = 0;
  auto y = 0;
  auto z = 0;
  for (const auto &R_3D : sig_lattice_list) {
    x = std::max(x, R_3D(0));
    y = std::max(y, R_3D(1));
    z = std::max(z, R_3D(2));
  }
  R_max_ = Vector3i({x, y, z});
  R_size_ = 1 + direct_ord_idx(R_max_, R_max_);
  ExEnv::out0() << "Updated range of lattice sum for |mu nu_R) = "
                << R_max_.transpose() << std::endl;

  // do not forget to recompute significant shell pairs using new R_max_
  basisR_ptr = shift_basis_origin(*basis_ptr, Vector3d::Zero(), R_max_, dcell_);
  sig_shellpair_list_ =
      parallel_compute_shellpair_list(this->world(), *basis_ptr, *basisR_ptr,
                                      shell_pair_threshold_, engine_precision_);
}

#if TA_DEFAULT_POLICY == 1
extern template class PeriodicAOFactory<TA::TensorD, TA::SparsePolicy>;
#endif

}  // namespace gaussian
}  // namespace lcao
}  // namespace mpqc
#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_LCAO_FACTORY_PERIODIC_AO_FACTORY_H_
