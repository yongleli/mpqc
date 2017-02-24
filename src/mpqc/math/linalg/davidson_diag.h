//
// Created by ChongPeng on 2/20/17.
//

#ifndef SRC_MPQC_MATH_LINALG_DAVIDSON_DIAG_H_
#define SRC_MPQC_MATH_LINALG_DAVIDSON_DIAG_H_

#include "mpqc/math/external/eigen/eigen.h"
#include "mpqc/math/linalg/gram_schmidt.h"
#include "mpqc/util/misc/exenv.h"

#include <TiledArray/algebra/utils.h>
#include <mpqc/util/misc/assert.h>
#include <tiledarray.h>

namespace mpqc {

template <typename Tile, typename Policy>
inline void plus(TA::DistArray<Tile, Policy>& y,
                 const TA::DistArray<Tile, Policy>& x) {
  const std::string vars =
      TA::detail::dummy_annotation(y.trange().tiles_range().rank());
  y(vars) += x(vars);
}

/**
 * \brief Davidson Algorithm
 *
 * \tparam D array type
 *
 *
 */

template <typename D>
class SymmDavidsonDiag {
 public:
  using element_type = typename D::element_type;
  using result_type = EigenVector<element_type>;
  using value_type = std::vector<D>;

// private:
//  struct EigenPair {
//    element_type eigen_value;
//    result_type eigen_vector;
//
//    /// constructor
//    EigenPair(const element_type& value, const result_type& vector)
//        : eigen_value(value), eigen_vector(vector) {}
//
//    /// move constructor
//    EigenPair(const element_type&& value, const result_type&& vector)
//        : eigen_value(std::move(value)), eigen_vector(std::move(vector)) {}
//
//    ~EigenPair() = default;
//
//    // sort by eigen value
//    bool operator<(const EigenPair& other) const {
//      return eigen_value < other.eigen_value;
//    }
//  };

 public:
  /**
   *
   * @param n_roots number of lowest roots to solve
   * @param n_guess max number of guess vector
   */
  SymmDavidsonDiag(unsigned int n_roots, unsigned int n_guess)
      : n_roots_(n_roots), n_guess_(n_guess) {}

  /**
   *
   * @tparam Pred preconditioner object, which has void Pred(const element_type & e,
   * D& residual) to update residual
   *
   * @param HB product with A and guess vector
   * @param B  guess vector
   * @param pred preconditioner
   * @return B updated guess vector
   */
  template <typename Pred>
  EigenVector<element_type> extrapolate(value_type& HB, value_type& B,
                                        const Pred& pred) {
    TA_ASSERT(HB.size() == B.size());
    // size of vector
    const auto n_v = B.size();

    // subspace
    // dot_product will return a replicated Eigen Matrix
    RowMatrix<element_type> G(n_v, n_v);
    for (auto i = 0; i < n_v; ++i) {
      for (auto j = 0; j < n_v; ++j) {
        G(i, j) = dot_product(B[j], HB[i]);
      }
    }

    // do eigen solve locally
    result_type E(n_roots_);
    RowMatrix<element_type> C(n_v, n_roots_);
    {
      // this return eigenvalue and eigenvector with size n_guess
      Eigen::SelfAdjointEigenSolver<RowMatrix<element_type>> es(G);

        RowMatrix<element_type> v = es.eigenvectors();
        EigenVector<element_type> e = es.eigenvalues();

        //        std::cout << es.eigenvalues() << std::endl;

      E = e.segment(0,n_roots_);
      C = v.leftCols(n_roots_);

    }

    // compute residual
    value_type residual(n_roots_);
    for (auto i = 0; i < n_roots_; ++i) {
      // initialize residual as 0
      residual[i] = copy(B[i]);
      zero(residual[i]);
      const auto e_i = -E[i];

      for (auto j = 0; j < n_v; ++j) {
        D tmp = copy(residual[i]);
        zero(tmp);
        axpy(tmp, e_i, B[j]);
        plus(tmp, HB[j]);
        scale(tmp, C(j, i));
        plus(residual[i], tmp);
      }
    }

    // precondition
    for(auto i = 0; i < n_roots_; i++){
      pred(E[i], residual[i]);
    }

    // extra vector
    //    unsigned int n_extra = n_guess_ - (n_v + n_roots_);
    // delete first n_extra vector
    //    if( n_extra > 0 ){
    //
    //    }

    B.insert(B.end(), residual.begin(), residual.end());

    // orthognolize new residual with original B
    gram_schmidt(B, n_v);

    return E.segment(0, n_roots_);
  }

 private:
  unsigned int n_roots_;
  unsigned int n_guess_;
};

}  // namespace mpqc

#endif  // SRC_MPQC_MATH_LINALG_DAVIDSON_DIAG_H_
