

#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_SCREENING_SCHWARZ_SCREEN_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_SCREENING_SCHWARZ_SCREEN_H_

#include <tiledarray.h>

#include "mpqc/chemistry/qc/integrals/screening/screen_base.h"
#include "mpqc/chemistry/qc/integrals/task_integrals_common.h"

#include <stdexcept>
#include <unordered_map>

#include <boost/optional>

namespace mpqc {
namespace lcao {
namespace gaussian {

/*! \brief Class which holds shell set information for screening.
 *
 * Must keep a function to shell map which takes the index of a function and
 * returns the index of its shell.  The map only maps the first function in
 * each shell so should fail if say the global index of the second function in
 * the shell is passed in.
 */
class Qmatrix {
 private:
  RowMatrixXd Q_;
  std::unordered_map<int64_t, int64_t> func_to_shell_map0_;
  std::unordered_map<int64_t, int64_t> func_to_shell_map1_;
  Eigen::VectorXd max_elem_in_row_;
  double max_elem_in_Q_;

  std::pair<int64_t, int64_t> f2s(int64_t a, int64_t b) const {
    auto it0 = func_to_shell_map0_.find(a);
    auto it1 = func_to_shell_map1_.find(b);

    // If this hits the issue was likely an index that was not the
    // first function in the shell.
    assert(it0 != func_to_shell_map0_.end());
    assert(it1 != func_to_shell_map1_.end());
    return std::make_pair(it0->second, it1->second);
  }

  int64_t f2s0(int64_t a) const {
    auto it0 = func_to_shell_map0_.find(a);

    // If this hits the issue was likely an index that was not the
    // first function in the shell.
    assert(it0 != func_to_shell_map0_.end());
    return it0->second;
  }

  int64_t f2s1(int64_t a) const {
    auto it1 = func_to_shell_map1_.find(a);

    // If this hits the issue was likely an index that was not the
    // first function in the shell.
    assert(it1 != func_to_shell_map1_.end());
    return it1->second;
  }

 public:
  Qmatrix() = default;
  Qmatrix(Qmatrix const &) = default;
  Qmatrix(Qmatrix &&) = default;
  Qmatrix &operator=(Qmatrix const &) = default;
  Qmatrix &operator=(Qmatrix &&) = default;
  Qmatrix(RowMatrixXd Q, std::unordered_map<int64_t, int64_t>);
  Qmatrix(RowMatrixXd Q, std::unordered_map<int64_t, int64_t>,
          std::unordered_map<int64_t, int64_t>);

  // Get whole matrix
  RowMatrixXd const &Q() const { return Q_; }

  // If vector get index
  double const &Q(int64_t idx) const { return Q_(f2s0(idx)); }

  // If Matrix get elem
  double const &Q(int64_t row, int64_t col) const {
    const auto idx = f2s(row, col);
    return Q_(idx.first, idx.second);
  }

  double const &max_in_row(int64_t row) const {
    return max_elem_in_row_(f2s0(row));
  }

  double max_in_Q() const { return max_elem_in_Q_; }

  // int64_t func_to_shell(int64_t func_idx) const { return f2s(func_idx); }
};

/*! \brief Class for Schwarz based screening.
 *
 * We will assume that these screeners are replicated for the integrals they
 * need and so will not bother with serialization of them.
 *
 * We need to hold two Q matrices because sometimes we may want to screen from
 * different bases in the bra and ket, one example being Density Fitting.
 */
class SchwarzScreen : public Screener {
 private:
  std::shared_ptr<Qmatrix> Qab_;
  std::shared_ptr<Qmatrix> Qcd_;
  double thresh_;

 public:
  bool Qab_is_vector() const { return Qab_->Q().cols() == 1; }

  const double &Qab(int64_t a) const {
    assert(Qab_ != nullptr && Qab_->Q().cols() == 1);
    return Qab_->Q(a);
  }

  const double &Qcd(int64_t a) const {
    assert(Qcd_ != nullptr && Qcd_->Q().cols() == 1);
    return Qab_->Q(a);
  }

  const double &Qab(int64_t a, int64_t b) const {
    assert(Qab_ != nullptr);
    return Qab_->Q(a, b);
  }

  const double &Qcd(int64_t c, int64_t d) const {
    assert(Qcd_ != nullptr);
    return Qcd_->Q(c, d);
  }

  double max_ab() const {
    assert(Qab_ != nullptr);
    return Qab_->max_in_Q();
  }

  double max_cd() const {
    assert(Qcd_ != nullptr);
    return Qcd_->max_in_Q();
  }

  double max_in_row_ab(int64_t row_a) const {
    assert(Qab_ != nullptr);
    return Qab_->max_in_row(row_a);
  }

  double max_in_row_cd(int64_t row_a) const {
    assert(Qcd_ != nullptr);
    return Qcd_->max_in_row(row_a);
  }

  // Screen three center based on first index
  double three_center_est(int64_t a) const { return Qab(a) * max_cd(); }

  double three_center_est(int64_t a, int64_t b) const {
    return Qab(a) * max_in_row_cd(b);
  }

  double three_center_est(int64_t a, int64_t b, int64_t c) const {
    return Qab(a) * Qcd(b, c);
  }

  // Here for interface reasons not actually usable
  double three_center_est(int64_t, int64_t, int64_t, int64_t) const {
    assert(false);
    return 0.0;
  }

  double four_center_est(int64_t a) const {
    return max_in_row_ab(a) * max_cd();
  }

  double four_center_est(int64_t a, int64_t b) const {
    return Qab(a, b) * max_cd();
  }

  double four_center_est(int64_t a, int64_t b, int64_t c) const {
    return Qab(a, b) * max_in_row_cd(c);
  }

  double four_center_est(int64_t a, int64_t b, int64_t c, int64_t d) const {
    return Qab(a, b) * Qcd(c, d);
  }

  /// Returns the shell index given the starting funciton index of that shell.
  int64_t Qab_f2s(int64_t func_idx) const {
    return Qab_->func_to_shell(func_idx);
  }

  /// Returns the shell index given the starting funciton index of that shell.
  int64_t Qcd_f2s(int64_t func_idx) const {
    return Qcd_->func_to_shell(func_idx);
  }

  template <typename... IDX>
  double screen(IDX... idx) const {
    if (Qab_is_vector()) {  // Three Center Integrals
      assert(Qcd_ != nullptr);
      assert(sizeof...(IDX) <= 3);
      return three_center_est(idx...);
    } else {  // Four center with 2 matrices
      return four_center_est(idx...);
    }
  }

  SchwarzScreen() = default;
  SchwarzScreen(SchwarzScreen const &) = default;
  SchwarzScreen(SchwarzScreen &&) = default;
  SchwarzScreen &operator=(SchwarzScreen const &) = default;
  SchwarzScreen &operator=(SchwarzScreen &&) = default;

  virtual ~SchwarzScreen() noexcept = default;

  /*! \brief Constructor which requires a Q matrix.
   *
   * The threshold for screening defaults to 1e-10, but is settable,
   *
   * For Density Fitting integrals of type (A|cd) Qab_ represents A and
   * Qcd_ represents c and d, The reverse ordering is not supported.
   * Qab_ will be a vector.
   */
  SchwarzScreen(std::shared_ptr<Qmatrix> Qab, std::shared_ptr<Qmatrix> Qcd,
                double thresh = 1e-10)
      : Screener(),
        Qab_(std::move(Qab)),
        Qcd_(std::move(Qcd)),
        thresh_(thresh) {}

  double skip_threshold() const { return thresh_; }

  // Not overriding the two index parts

  // Assume for all three center that ordering is (A|cd)
  bool skip(int64_t a) override { return (screen(a) < thresh_) ? true : false; }

  bool skip(int64_t a, int64_t b) override {
    return (screen(a, b) < thresh_) ? true : false;
  }

  bool skip(int64_t a, int64_t b, int64_t c) override {
    return (screen(a, b, c) < thresh_) ? true : false;
  }

  bool skip(int64_t a, int64_t b, int64_t c, int64_t d) override {
    return (screen(a, b, c, d) < thresh_) ? true : false;
  }
};

namespace detail {

// Make map to shell idx given the first function in the shell.
inline std::unordered_map<int64_t, int64_t> func_to_shell(
    std::vector<Shell> const &shells) {
  std::unordered_map<int64_t, int64_t> f2s;
  const auto size = shells.size();

  auto func_idx = 0;
  for (auto i = 0ul; i < size; ++i) {
    f2s.emplace(func_idx, i);
    func_idx += shells[i].size();
  }

  return f2s;
}

// Compute Q vector
template <typename E>
inline RowMatrixXd auxiliary_Q(madness::World &world, ShrPool<E> const &eng,
                               std::vector<Shell> const &shells) {
  // pass by pointers since tasks copy params
  auto task_func = [=](Shell const *sh, Shell const *ush, double *Q_val) {
    const auto &bufs = eng->local().compute(*sh, *ush, *sh, *ush);
    TA_USER_ASSERT(bufs.size() == 1, "unexpected result from Engine::compute");
    const auto nsh = sh->size();
    const auto bmap = Eigen::Map<const RowMatrixXd>(bufs[0], nsh, nsh);

    *Q_val = std::sqrt(bmap.lpNorm<2>());
  };

  const auto nshells = shells.size();
  Eigen::VectorXd Q(nshells);

  auto const *ush = &unit_shell;
  for (auto i = 0ul; i < nshells; ++i) {
    auto *Q_val_ptr = &Q(i);
    world.taskq.add(task_func, &shells[i], ush, Q_val_ptr);
  }
  world.gop.fence();

  return Q;
}

// Compute Q matrix
template <typename E>
inline RowMatrixXd four_center_Q(madness::World &world, ShrPool<E> const &eng,
                                 std::vector<Shell> const &shells) {
  auto task_func = [=](Shell const *sh0, Shell const *sh1, double *Q_val) {
    eng->local().set_precision(0.);
    const auto &bufs = eng->local().compute(*sh0, *sh1, *sh0, *sh1);
    TA_USER_ASSERT(bufs.size() == 1, "unexpected result from Engine::compute");

    const auto n2 = sh0->size() * sh1->size();
    const auto bmap = Eigen::Map<const RowMatrixXd>(bufs[0], n2, n2);

    eng->local().set_precision(std::numeric_limits<double>::epsilon());

    *Q_val = std::sqrt(bmap.lpNorm<2>());
  };

  const auto nshells = shells.size();
  RowMatrixXd Q(nshells, nshells);

  for (auto i = 0ul; i < nshells; ++i) {
    for (auto j = 0ul; j < nshells; ++j) {
      auto *Q_val_ptr = &Q(i, j);
      world.taskq.add(task_func, &shells[i], &shells[j], Q_val_ptr);
    }
  }

  world.gop.fence();

  return Q;
}

// Compute Q
template <typename E>
inline std::shared_ptr<Qmatrix> compute_Q(madness::World &world,
                                          ShrPool<E> const &eng,
                                          Basis const &bs,
                                          bool auxiliary_basis = false) {
  const auto shells = bs.flattened_shells();
  RowMatrixXd Q;
  if (auxiliary_basis) {
    Q = auxiliary_Q(world, eng, shells);
  } else {
    Q = four_center_Q(world, eng, shells);
  }

  return std::make_shared<Qmatrix>(std::move(Q), func_to_shell(shells));
}

// Compute Q will create a Q matrix for two basis sets.
template <typename E>
inline std::shared_ptr<Qmatrix> compute_Q(madness::World &world,
                                          ShrPool<E> const &eng,
                                          Basis const &bs0, Basis const &bs1) {
  const auto shells0 = bs0.flattened_shells();
  const auto shells1 = bs1.flattened_shells();

  return std::make_shared<Qmatrix>(four_center_Q(world, eng, shells0, shells1),
                                   func_to_shell(shells0),
                                   func_to_shell(shells1));
}

// This overload of Compute Q will create a Q matrix for one basis sets. Usually
// this will be used for auxiliary bases
template <typename E>
inline std::shared_ptr<Qmatrix> compute_Q(madness::World &world,
                                          ShrPool<E> const &eng,
                                          Basis const &bs0) {
  const auto shells = bs.flattened_shells();
  return std::make_shared<Qmatrix>(auxiliary_Q(world, eng, shells),
                                   func_to_shell(shells));
}

}  // namespace  detail;

/*! \brief struct which builds SchwarzScreen screeners */
class init_schwarz_screen {
 private:
  double threshold = 1e-12;

  // Here we must assume which basis is the auxiliary basis.  For now assume
  // that the auxiliary basis is the first one.
  template <typename E>
  SchwarzScreen compute_df(
      madness::World &world, ShrPool<E> &eng,
      std::vector<lcao::gaussian::Basis> const &bs_array) const {
    auto Q_a = detail::compute_Q(world, eng, dfbs, true);
    auto Q_cd = detail::compute_Q(world, eng, obs);

    return SchwarzScreen(std::move(Q_a), std::move(Q_cd), threshold);
  }

  // template <typename E>
  // SchwarzScreen compute_4c(madness::World &world, ShrPool<E> &eng,
  //                          Basis const &bs) const {
  //   auto Q_ab = detail::compute_Q(world, eng, bs);
  //   return SchwarzScreen(std::move(Q_ab), nullptr, threshold);
  // }

  template <typename E>
  SchwarzScreen compute_4c(
      madness::World &world, ShrPool<E> &eng,
      std::vector<lcao::gaussian::Basis> const &bs_array) const {
    auto Q_ab = detail::compute_Q(world, eng, bs_array[0]);
    return SchwarzScreen(std::move(Q_ab), nullptr, threshold);
  }

 public:
  init_schwarz_screen() = default;
  init_schwarz_screen(double thresh) : threshold(thresh) {}

  template <typename E>
  SchwarzScreen operator()(madness::World &world, ShrPool<E> &eng,
                           Basis const &bs) const {
    return compute_4c(world, eng, bs);
  }

  template <typename E>
  SchwarzScreen operator()(madness::World &world, ShrPool<E> &eng,
                           std::vector<lcao::gaussian::Basis> const &bs_array,
                           bool mixed_basis_four_center = false) const {
    const auto nbasis = bs_array.size();
    if (nbasis == 4) {
      return compute_4c(world, eng, bs_array);
    } else if (nbasis == 3) {
      return compute_df(world, eng, bs_array);
    }
  }
};

/*! \brief Class which holds shell set information for screening.
 *
 * Qmatrix should contain a function to shell map for each basis it represents
 *
 */
class QmatrixP {
 private:
  using f2s_map = std::unordered_map<int64_t, int64_t>;
  std::array<f2s_map, 2> f2s_maps; // Function to shell maps for each basis

  RowMatrixXd Q_; // Matrix to hold the integral estimates
  Eigen::VectorXd max_elem_in_row_; // Max element for each row of Q_
  double max_elem_in_Q_; // Max val in Q_


 public:
  QmatrixP() = default;
  QmatrixP(QmatrixP const &) = default;
  QmatrixP(QmatrixP &&) = default;
  QmatrixP &operator=(QmatrixP const &) = default;
  QmatrixP &operator=(QmatrixP &&) = default;
};

/*! \brief Class for Schwarz based screening.
 *
 * We will assume that these screeners are replicated for the integrals they
 * need and so will not bother with serialization of them.
 *
 * SchwarzScreen has support for up to 4 different bases, currently there are
 * no optimizations made for all four bases being equal to each other.
 */
class SchwarzScreenP : public Screener {
 private:
  std::shared_ptr<QmatrixP> Qab_;
  std::shared_ptr<QmatrixP> Qcd_;
  double thresh_;

  // estimate returns an optional double.  If estimate knows how to estimate
  // the integral provided it the optional contains the estimation, otherwise
  // the optional is empty. idx should be a list of function indices for the
  // integral set we which to estimate
  template <typename... IDX>
  boost::optional<double> estimate(IDX... idx) const {
    return boost::none;
  }

 public:
  SchwarzScreenP() = default;
  SchwarzScreenP(SchwarzScreenP const &) = default;
  SchwarzScreenP(SchwarzScreenP &&) = default;
  SchwarzScreenP &operator=(SchwarzScreenP const &) = default;
  SchwarzScreenP &operator=(SchwarzScreenP &&) = default;

  virtual ~SchwarzScreenP() noexcept = default;

  /*! \brief Constructor which requires two Q matrices
   *
   * \param Qab is the Qmatrix needed for mu and nu in (mu nu | rho sigma) and
   * also the vector needed for X in (X | rho sigma), a current limitation of
   * Schwarz screening is that all auxillary basis Qs must be placed in Qab_.
   *
   * \param Qcd is the Qmatrix needed for rho and sigma in (mu nu | rho sigma)
   * and (X | rho sigma)
   *
   * \param thresh is the screening threshold used when evaluating whether or
   * not the skip function returns true for Qab(a,b) * Qcd(c,d) <= thresh_.
   * How Qab and Qcd are determined (infinity norm of a shell set or F norm) is
   * to be part of the Q matrix construction.
   */
  SchwarzScreenP(std::shared_ptr<Qmatrix> Qab, std::shared_ptr<Qmatrix> Qcd,
                 double thresh = 1e-12);

  /// Reports the threshold being used for skipping integrals
  double skip_threshold() const;

  /*! skip takes a list of indices and returns true when the integral estimate
   * is below the threshold and false when it is greater than or equal to the
   * threshold. See function estimate for the implementation.
   *
   * \param idx is a series of indices from which to generate an integral
   * estimate
   */
  template <typename... IDX>
  bool skip(IDX... idx) const {
    auto est = estimate(idx);  // optional estimation for set idx
    if (est) {  // Check that we were able to estimate the integral set
      return est < thresh_;  // If est below threshold then skip this set
    } else {         // We were unable to estimate this set for some reason
      return false;  // thus we should compute the integral
    }
  }
};

}  // namespace  gaussian
}  // namespace  lcao
}  // namespace  mpqc

#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_INTEGRALS_SCREENING_SCHWARZ_SCREEN_H_
