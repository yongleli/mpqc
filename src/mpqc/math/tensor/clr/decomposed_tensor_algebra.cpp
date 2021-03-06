#include "mpqc/math/tensor/clr/decomposed_tensor_algebra.h"

#include "mpqc/math/tensor/clr/decomposed_tensor.h"

namespace mpqc {
namespace math {

integer col_pivoted_qr(double *data, double *Tau, integer rows, integer cols,
                       integer *J) {
  double work_dummy;
  integer LWORK = -1;  // Ask for space computation
  integer INFO;
  integer LDA = rows;

  // Call routine
  dgeqp3_(&rows, &cols, data, &LDA, J, Tau, &work_dummy, &LWORK, &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);
  dgeqp3_(&rows, &cols, data, &LDA, J, Tau, W.get(), &LWORK, &INFO);
  return INFO;
}

integer non_pivoted_qr(double *data, double *Tau, integer rows, integer cols) {
  double work_dummy;
  integer LWORK = -1;  // Ask for space computation
  integer INFO;
  integer LDA = rows;

  // Call routine
  dgeqrf_(&rows, &cols, data, &LDA, Tau, &work_dummy, &LWORK, &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);
  dgeqrf_(&rows, &cols, data, &LDA, Tau, W.get(), &LWORK, &INFO);
  return INFO;
}

integer non_pivoted_lq(double *data, double *Tau, integer rows, integer cols) {
  double work_dummy;
  integer LWORK = -1;  // Ask for space computation
  integer INFO;
  integer LDA = rows;

  // Call routine
  dgelqf_(&rows, &cols, data, &LDA, Tau, &work_dummy, &LWORK, &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);
  dgelqf_(&rows, &cols, data, &LDA, Tau, W.get(), &LWORK, &INFO);
  return INFO;
}

integer svd(double *data, double *s, double *u, double *vt, integer rows,
            integer cols) {
  double work_dummy;
  integer LWORK = -1;  // Ask for space computation
  integer INFO;
  integer LDA = rows;
  integer LDU = (rows < cols) ? rows : 1;
  integer LDVT = (rows >= cols) ? cols : 1;
  auto iwork = std::make_unique<integer[]>(8 * std::min(rows, cols));

  // Call routine
#ifndef MADNESS_LINALG_USE_LAPACKE
  const char O = 'O';
#else
  char O = 'O';
#endif
  dgesdd_(&O, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, &work_dummy,
          &LWORK, iwork.get(), &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);

  dgesdd_(&O, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, W.get(), &LWORK,
          iwork.get(), &INFO);
  return INFO;
}

integer svd(double *data, double *s, double *u, double *vt, integer rows,
            integer cols, const char JOBZ) {
  double work_dummy;
  integer LWORK = -1;  // Ask for space computation
  integer INFO;
  integer LDA = rows;
  integer LDU, LDVT;
  switch (JOBZ) {
  case 'A':
    LDU = rows;
    LDVT = cols;
    break;
  case 'S':
    LDU = std::min(rows, cols);
    LDVT = LDU;
    break;
  case 'O':
    LDU = 1;
    LDVT = cols;
    break;
  case 'N':
    LDU = 1;
    LDVT = 1;
    break;
  default:
    std::cout << "Invalid input for JOBZ" << std::endl;
  }

  auto iwork = std::make_unique<integer[]>(8 * std::min(rows, cols));

  // Call routine
#ifndef MADNESS_LINALG_USE_LAPACKE
  dgesdd_(&JOBZ, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, &work_dummy,
          &LWORK, iwork.get(), &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);
  dgesdd_(&JOBZ, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, W.get(), &LWORK,
          iwork.get(), &INFO);
#else
  char jobz = JOBZ;
  dgesdd_(&jobz, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, &work_dummy,
          &LWORK, iwork.get(), &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto W = std::make_unique<double[]>(LWORK);
  dgesdd_(&jobz, &rows, &cols, data, &LDA, s, u, &LDU, vt, &LDVT, W.get(), &LWORK,
          iwork.get(), &INFO);
#endif
  return INFO;
}

integer form_q(double *data, double *Tau, integer rows, integer rank) {
  double work_dummy = 0.0;
  integer LWORK = -1;
  integer INFO;
  dorgqr_(&rows, &rank, &rank, data, &rows, Tau, &work_dummy, &LWORK, &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto work = std::make_unique<double[]>(LWORK);

  dorgqr_(&rows, &rank, &rank, data, &rows, Tau, work.get(), &LWORK, &INFO);

  return INFO;
}

integer form_q_from_lq(double *data, double *Tau, integer cols, integer rows,
                       integer rank) {
  double work_dummy = 0.0;
  integer LWORK = -1;
  integer INFO;
  dorglq_(&rank, &cols, &rank, data, &rows, Tau, &work_dummy, &LWORK, &INFO);
  assert(INFO == 0);
  LWORK = work_dummy;
  auto work = std::make_unique<double[]>(LWORK);

  dorglq_(&rank, &cols, &rank, data, &rows, Tau, work.get(), &LWORK, &INFO);

  return INFO;
}

size_t svd_rank(double const *s, size_t N, double thresh) {
  auto rank = 0;
  for (auto i = 0u; i < N; ++i) {
    if (s[i] >= thresh) {
      ++rank;
    } else {
      break;
    }
  }

  return std::max(rank, 1);
}

// calculate the qr_rank of a matrix.
std::size_t qr_rank(double const *data, std::size_t rows, std::size_t cols,
                    double threshold) {
  const auto full_rank = std::min(cols, rows);
  auto out_rank = full_rank;

  auto M = Eigen::Map<const Eigen::MatrixXd>(data, rows, cols);

  auto squared_sum = 0.0;
  auto thresh2 = threshold * threshold;
  for (int i = (full_rank - 1); i >= 0; --i) {  // rows of R
    for (int j = (cols - 1); j >= i; --j) {     // cols of R
      squared_sum += M(i, j) * M(i, j);
    }

    if (squared_sum >= thresh2) {
      return out_rank;
    }

    --out_rank;  // Decriment rank and go to next row.
  }

  return std::max(out_rank, 1ul);
}

// Returns true if input is low rank.
bool full_rank_decompose(TA::Tensor<double> const &in, TA::Tensor<double> &L,
                         TA::Tensor<double> &R, double thresh,
                         std::size_t max_out_rank) {
  auto const extent = in.range().extent();
  auto dims = in.range().rank();

  int cols = extent[0];
  int rows = 1;
  for (auto i = 1ul; i < dims; ++i) {
    rows *= extent[i];
  }
  const auto size = cols * rows;
  auto full_rank = std::min(rows, cols);

  // Will hold the column pivot information and reflectors
  typedef Eigen::Matrix<integer, Eigen::Dynamic, 1> VectorXi;
  VectorXi J = VectorXi::Zero(cols);
  auto Tau = std::make_unique<double[]>(full_rank);

  auto in_data = std::make_unique<double[]>(size);
  std::copy(in.data(), in.data() + size, in_data.get());

  // Do initial qr routine
  auto qr_err = col_pivoted_qr(in_data.get(), Tau.get(), rows, cols, J.data());
  if (0 != qr_err) {
    assert(qr_err < 0);
    std::cout << "problem with " << qr_err
              << "th argument to LAPACK in col_pivoted_qr.\n";
    throw;
  }

  // Determine rank and if decomposing is worth it.
  auto rank = qr_rank(in_data.get(), rows, cols, thresh);
  if (max_out_rank == std::numeric_limits<std::size_t>::max()) {
    max_out_rank = full_rank / 2;
  }
  if (rank >= max_out_rank) {
    return false;
  }

  // LAPACK assumes 1 based indexing, but we need zero.
  std::for_each(J.data(), J.data() + J.size(), [](integer &val) { --val; });
  Eigen::PermutationWrapper<VectorXi> P(J);

  // Create L tensor
  TA::Range l_range(extent[0], static_cast<std::size_t>(rank));
  L = TA::Tensor<double>(std::move(l_range));

  // Eigen map the input
  auto A = Eigen::Map<Eigen::MatrixXd>(in_data.get(), rows, cols);

  // Assign into l_tensor
  auto L_map = Eigen::Map<Eigen::MatrixXd>(L.data(), rank, cols);
  L_map =
      Eigen::MatrixXd(
          A.topLeftCorner(rank, cols).template triangularView<Eigen::Upper>()) *
      P.transpose();

  auto q_err = form_q(in_data.get(), Tau.get(), rows, rank);
  if (0 != q_err) {
    std::cout << "Something went wrong with forming q.\n";
    throw;
  }

  // From Q goes to R because of column major transpose issues.
  if (dims == 3) {
    TA::Range q_range{static_cast<std::size_t>(rank), extent[1], extent[2]};
    R = TA::Tensor<double>(std::move(q_range));
  } else if (dims == 2) {
    TA::Range q_range{static_cast<std::size_t>(rank), extent[1]};
    R = TA::Tensor<double>(std::move(q_range));
  } else {
    assert(false);
  }

  auto R_map = Eigen::Map<Eigen::MatrixXd>(R.data(), rows, rank);
  R_map = A.leftCols(rank);
  return true;
}

void ta_tensor_col_pivoted_qr(TA::Tensor<double> &in, TA::Tensor<double> &L,
                              TA::Tensor<double> &R, double thresh) {
  auto const extent = in.range().extent();

  int cols = extent[0];
  int rows = 1;
  for (auto i = 1ul; i < extent.size(); ++i) {
    rows *= extent[i];
  }
  auto full_rank = std::min(rows, cols);

  // Will hold the column pivot information and reflectors
  typedef Eigen::Matrix<integer, Eigen::Dynamic, 1> VectorXi;
  VectorXi J = VectorXi::Zero(cols);
  auto Tau = std::make_unique<double[]>(full_rank);

  // Do initial qr routine
  auto qr_err = col_pivoted_qr(in.data(), Tau.get(), rows, cols, J.data());
  if (0 != qr_err) {
    assert(qr_err < 0);
    std::cout << "problem with " << qr_err
              << "th argument to LAPACK in col_pivoted_qr.\n";
    throw;
  }

  // Determine rank and if decomposing is worth it.
  auto rank = qr_rank(in.data(), rows, cols, thresh);

  // LAPACK assumes 1 based indexing, but we need zero.
  std::for_each(J.data(), J.data() + J.size(), [](integer &val) { --val; });
  Eigen::PermutationWrapper<VectorXi> P(J);

  // Create L tensor
  TA::Range l_range{extent[0], static_cast<std::size_t>(rank)};
  L = TA::Tensor<double>(std::move(l_range));

  // Eigen map the input
  auto A = Eigen::Map<Eigen::MatrixXd>(in.data(), rows, cols);

  // Assign into l_tensor
  auto L_map = Eigen::Map<Eigen::MatrixXd>(L.data(), rank, cols);
  L_map =
      Eigen::MatrixXd(
          A.topLeftCorner(rank, cols).template triangularView<Eigen::Upper>()) *
      P.transpose();

  auto q_err = form_q(in.data(), Tau.get(), rows, rank);

  if (0 != q_err) {
    std::cout << "Something went wrong with forming q.\n";
    throw;
  }

  // Form Q goes to R because of column major transpose issues.
  if (extent.size() == 3) {
    TA::Range q_range{static_cast<std::size_t>(rank), extent[1], extent[2]};
    R = TA::Tensor<double>(std::move(q_range));
  } else if (extent.size() == 2) {
    TA::Range q_range{static_cast<std::size_t>(rank), extent[1]};
    R = TA::Tensor<double>(std::move(q_range));
  } else {
    assert(false);
  }

  auto R_map = Eigen::Map<Eigen::MatrixXd>(R.data(), rows, rank);
  R_map = A.leftCols(rank);
}

// eats in data and outputs L and R tensors.
void ta_tensor_qr(TA::Tensor<double> &in, TA::Tensor<double> &L,
                  TA::Tensor<double> &R) {
  auto const extent = in.range().extent();

  // Reverse map
  // We will always extract the first dimension as cols
  int cols = extent[0];

  // The rest will be smashed into rows
  const auto ndims = extent.size();
  int rows = 1;
  for (auto i = 1ul; i < ndims; ++i) {
    rows *= extent[i];
  }

  auto full_rank = std::min(rows, cols);

  // Will hold the reflectors
  auto Tau = std::make_unique<double[]>(full_rank);

  // Lets start by not copying data
  /* const auto size = cols * rows; */
  /* auto in_data = std::make_unique<double[]>(size); */
  /* std::copy(in.data(), in.data() + size, in_data.get()); */

  auto qr_err = non_pivoted_qr(in.data(), Tau.get(), rows, cols);

  if (0 != qr_err) {
    assert(qr_err < 0);
    std::cout << "problem with " << qr_err
              << "th argument to LAPACK in non_pivoted_qr.\n";
    throw;
  }

  TA::Range l_range{static_cast<unsigned int>(cols),
                    static_cast<unsigned long>(full_rank)};
  L = TA::Tensor<double>(std::move(l_range));

  // Eigen map the input
  auto A = Eigen::Map<Eigen::MatrixXd>(in.data(), rows, cols);

  // Assign into l_tensor
  auto L_map = Eigen::Map<Eigen::MatrixXd>(L.data(), full_rank, cols);
  L_map =
      A.topLeftCorner(full_rank, cols).template triangularView<Eigen::Upper>();

  auto q_err = form_q(in.data(), Tau.get(), rows, full_rank);
  if (0 != q_err) {
    std::cout << "Something went wrong with forming q.\n";
    throw;
  }

  if (ndims == 2) {
    TA::Range q_range{static_cast<unsigned int>(full_rank), extent[1]};
    R = TA::Tensor<double>(std::move(q_range));
  } else if (ndims == 3) {
    TA::Range q_range{static_cast<unsigned int>(full_rank), extent[1],
                      extent[2]};
    R = TA::Tensor<double>(std::move(q_range));
  } else {
    assert(false);
  }

  auto R_map = Eigen::Map<Eigen::MatrixXd>(R.data(), rows, full_rank);
  R_map = A.leftCols(full_rank);
}

// eats in data and outputs L and R tensors.
void ta_tensor_lq(TA::Tensor<double> &in, TA::Tensor<double> &L,
                  TA::Tensor<double> &R) {
  auto const extent = in.range().extent();

  // Reverse map
  // We will always extract the first dimension as cols
  int cols = extent[0];

  // The rest will be smashed into rows
  const auto ndims = extent.size();
  int rows = 1;
  for (auto i = 1ul; i < ndims; ++i) {
    rows *= extent[i];
  }

  auto full_rank = std::min(rows, cols);

  // Will hold the reflectors
  auto Tau = std::make_unique<double[]>(full_rank);
  auto qr_err = non_pivoted_lq(in.data(), Tau.get(), rows, cols);
  if (0 != qr_err) {
    assert(qr_err < 0);
    std::cout << "problem with " << qr_err
              << "th argument to LAPACK in non_pivoted_lq.\n";
    throw;
  }

  if (ndims == 2) {
    TA::Range q_range{static_cast<unsigned int>(full_rank), extent[1]};
    R = TA::Tensor<double>(std::move(q_range));
  } else if (ndims == 3) {
    TA::Range q_range{static_cast<unsigned int>(full_rank), extent[1],
                      extent[2]};
    R = TA::Tensor<double>(std::move(q_range));
  } else {
    assert(false);
  }

  auto A = Eigen::Map<Eigen::MatrixXd>(in.data(), rows, cols);

  auto R_map = Eigen::Map<Eigen::MatrixXd>(R.data(), rows, full_rank);
  R_map = A.bottomLeftCorner(rows, full_rank)
              .template triangularView<Eigen::Lower>();

  auto q_err = form_q_from_lq(in.data(), Tau.get(), cols, rows, full_rank);
  if (0 != q_err) {
    std::cout << "Something went wrong with forming q.\n";
    throw;
  }

  TA::Range l_range{static_cast<unsigned int>(cols),
                    static_cast<unsigned int>(full_rank)};
  L = TA::Tensor<double>(std::move(l_range));

  Eigen::Map<Eigen::MatrixXd> L_map(L.data(), full_rank, cols);
  L_map = A.topRows(full_rank);
}

// Sacrifice input data
void ta_tensor_svd(TA::Tensor<double> &in, TA::Tensor<double> &L,
                   TA::Tensor<double> &R, double thresh) {
  auto const extent = in.range().extent();
  auto ndims = extent.size();
  assert(ndims == 2);

  int cols = extent[0];
  int rows = 1;
  for (auto i = 1ul; i < ndims; ++i) {
    rows *= extent[i];
  }

  Eigen::VectorXd s(std::max(rows, cols));
  auto u = std::make_unique<double[]>((rows >= cols) ? 1 : rows * rows);
  auto vt = std::make_unique<double[]>((rows >= cols) ? cols * cols : 1);

  svd(in.data(), s.data(), u.get(), vt.get(), rows, cols);

  auto rank = svd_rank(s.data(), std::max(rows, cols), thresh);

  TA::Range l_range{extent[0], static_cast<std::size_t>(rank)};
  TA::Range r_range{static_cast<std::size_t>(rank), extent[1]};
  L = TA::Tensor<double>(std::move(l_range));
  R = TA::Tensor<double>(std::move(r_range));

  if (rows >= cols) {  // U is written into in
    auto Ldata = L.data();
    auto vt_ptr = vt.get();
    for (auto i = 0ul; i < extent[0]; ++i) {
      std::copy(vt_ptr, vt_ptr + rank, Ldata);
      vt_ptr += extent[0];
      Ldata += rank;
    }
    std::copy(in.data(), in.data() + R.range().volume(), R.data());

    // Roll in diagonal
    auto R_map = TA::eigen_map(R, rank, extent[1]);
    R_map = s.head(rank).asDiagonal() * R_map;
  } else {  // vt is written into in
    auto Ldata = L.data();
    auto vt_ptr = in.data();
    for (auto i = 0ul; i < extent[0]; ++i) {
      std::copy(vt_ptr, vt_ptr + rank, Ldata);
      vt_ptr += extent[0];
      Ldata += rank;
    }
    std::copy(u.get(), u.get() + R.range().volume(), R.data());

    // Roll in diagonal
    auto R_map = TA::eigen_map(R, rank, extent[1]);
    R_map = s.head(rank).asDiagonal() * R_map;
  }
}

/// Currently modifies input data regardless could cause some loss of accuracy.
void recompress(DecomposedTensor<double> &t) {
  assert(t.ndecomp() >= 2);

  // Given W = S * T;
  // we want S = Ls * Rs and T = Lt * Rt
  // W = Ls * Rs * Lt * Rt
  TA::Tensor<double> Ls, Rs, Lt, Rt;
  ta_tensor_lq(t.tensor(0), Ls, Rs);  // LQ Left to end up with QR since Row
                                      // Major
  ta_tensor_qr(t.tensor(1), Lt, Rt);  // QR right to end up with LQ since Row
                                      // Major

  // Form an M matrix where M = Rs * Lt
  constexpr auto NoT = madness::cblas::CBLAS_TRANSPOSE::NoTrans;
  const auto gh = TA::math::GemmHelper(NoT, NoT, 2, 2, 2);
  auto M = Rs.gemm(Lt, 1.0, gh);

  // want to always do the full decomp so make input
  // max rank larger than rank of M.
  TA::Tensor<double> Lm, Rm;

  // Form M = Lm * Rm
  ta_tensor_col_pivoted_qr(M, Lm, Rm, t.cut());

  // Form new Left side Snew = Ls * Lm
  auto newL = Ls.gemm(Lm, 1.0, gh);

  // Form new Right side Tnew = Rm * Rt
  auto ord = t.orders();
  const auto gh_r = TA::math::GemmHelper(NoT, NoT, ord[1], ord[0], ord[1]);
  auto newR = Rm.gemm(Rt, 1.0, gh_r);

  // W = Snew * Rnew
  t = DecomposedTensor<double>(t.cut(), std::move(newL), std::move(newR));
}

/// Returns an empty DecomposedTensor if the compression rank was to large.
DecomposedTensor<double> two_way_decomposition(
    DecomposedTensor<double> const &t) {
  if (t.ndecomp() >= 2) {
    assert(false);
  }

  auto const extent = t.tensor(0).range().extent();
  const auto rows = extent[0];
  auto second = extent.begin();
  std::advance(second, 1);
  const auto cols = std::accumulate(
      second, extent.end(), 1ul,
      [=](unsigned long val, decltype(extent[1]) next) { return val *= next; });
  const auto max_out_rank = std::min(rows, cols) / 2;
  TA::Tensor<double> L, R;
  if (full_rank_decompose(t.tensor(0), L, R, t.cut(), max_out_rank)) {
    return DecomposedTensor<double>(t.cut(), std::move(L), std::move(R));
  } else {
    return DecomposedTensor<double>{};
  }
}

TA::Tensor<double> combine(DecomposedTensor<double> const &t) {
  if (t.empty()) {
    return TA::Tensor<double>{};
  }

  const auto NoT = madness::cblas::CBLAS_TRANSPOSE::NoTrans;
  auto const &tensors = t.tensors();
  auto out = tensors[0].clone();
  for (auto i = 1ul; i < tensors.size(); ++i) {
    auto l_dim = out.range().rank();
    auto r_dim = tensors[i].range().rank();
    auto result_dim = l_dim + r_dim - 2;  // Only one contraction index.
    auto gh = TA::math::GemmHelper(NoT, NoT, result_dim, l_dim, r_dim);
    out = out.gemm(tensors[i], 1.0, gh);
  }

  return out;
}

/*&! \brief performs the pivoted Cholesky decomposition
 *
 * This function will take a matrix that is symmetric semi-positive definate
 * and over write it with the permuted and truncated Cholesky vectors
 * corresponding to the matrix.
*/
integer piv_cholesky(
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &a) {
  integer dim = a.rows();
  Eigen::Matrix<integer, Eigen::Dynamic, 1> piv =
      Eigen::Matrix<integer, Eigen::Dynamic, 1>::Zero(dim);

  integer rank = 0;
  integer info;
  double tol = -1;  // Use default tolerence if negative
  auto work = std::make_unique<double[]>(2 * dim);

  // Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> D =
  // a;
#ifndef MADNESS_LINALG_USE_LAPACKE
  const char uplo = 'U';
  dpstrf_(&uplo, &dim, a.data(), &dim, piv.data(), &rank, &tol, work.get(),
          &info);
#else
  char uplo = 'U';
  dpstrf_(&uplo, &dim, a.data(), &dim, piv.data(), &rank, &tol, work.get(),
          &info);
#endif

  // Fortran using 1 based indexing :(
  for (auto i = 0; i < piv.size(); ++i) {
    --piv[i];
  }

  Eigen::PermutationWrapper<decltype(piv)> P(piv);
  a = a.triangularView<Eigen::Lower>();
  // Eigen doesn't like you to assign to a with this expression directly
  typename std::remove_reference<decltype(a)>::type L = P * a.leftCols(rank);
  a = L;

  return rank;
}

}  // namespace math
}  // namespace mpqc
