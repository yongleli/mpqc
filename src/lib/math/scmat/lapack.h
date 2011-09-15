extern "C" {
#include <math/scmat/f77sym.h>

extern void F77_DGESVD(const char* jobu, const char* jobvt, const int* m,
                       const int* n, double* A, const int* lda, double* S, double* U, const int* ldu,
                       double* Vt, const int* ldvt, double* work, int* lwork, int* info);

extern void F77_DSPSVX(const char* fact, const char* uplo, const int* n, const int* nrhs,
                       const double* AP, double* AFP, int* ipiv, const double* BB, const int* nb,
                       double* XX, const int* nx, double* rcond, double* ferr, double* berr,
                       double* work, int* iwork, int* info);

extern void F77_DSYEVD(const char* need_evals, const char* uplo, const int* n,
                       double* Asq, const int* lda, double* evals, double* work, const int* lwork,
                       int* iwork, const int* liwork, int* info);

extern void F77_DSPTRF(const char* uplo, const int* n, double* AP, int* ipiv, int* info);

extern void F77_DPPTRF(const char* uplo, const int* n, double* AP, int* info);

extern void F77_DSPTRI(const char* uplo, const int* n, double* AP, const int* ipiv, double* work, int* info);

extern void F77_DPPTRI(const char* uplo, const int* n, double* AP, int* info);

extern double F77_DLANSP(const char* norm, const char* uplo, const int* n, const double* A_packed, double* work);

extern void F77_DSPCON(const char* uplo, const int* n, const double* A_packed, const int* ipiv,
                       const double* anorm, double* rcond, double* work, int* iwork, int* info);

extern void F77_DPPCON(const char* uplo, const int* n, const double* A_packed,
                       const double* anorm, double* rcond, double* work, int* iwork, int* info);

extern double F77_DLAMCH(const char* e);

extern void F77_DLACPY(const char* uplo, const int* m, const int* n, const double* A, const int* lda,
                       double* B, const int* ldb, int* info);

extern void F77_DSPTRS(const char* uplo, const int* n, const int* nrhs, const double* AFP, const int* ipiv,
                       const double* X, const int* ldx, int* info);

extern void F77_DPPTRS(const char* uplo, const int* n, const int* nrhs, const double* AFP,
                       const double* X, const int* ldx, int* info);

extern void F77_DSPRFS(const char* uplo, const int* n, const int* nrhs, const double* A, const double* AF,
                       const int* ipiv, const double* B, const int* ldb, const double* X,
                       const int* ldx, double* ferr, double* berr, double* work, int* iwork, int* info);

extern void F77_DPPRFS(const char* uplo, const int* n, const int* nrhs, const double* A, const double* AF,
                       const double* B, const int* ldb, const double* X,
                       const int* ldx, double* ferr, double* berr, double* work, int* iwork, int* info);

extern void F77_DSYGV(const int* itype, const char* jobz, const char* uplo, const int* n,
                      double* Asq, const int* lda, double* Bsq, const int* ldb, double* evals,
                      double* work, const int* lwork,
                      int* info);

}
