
#ifndef _math_scmat_matrix_h
#define _math_scmat_matrix_h
#ifdef __GNUC__
#pragma interface
#endif

#include <iostream.h>
#include <util/container/array.h>
#include <util/container/set.h>

#include <math/scmat/abstract.h>

class SCVectordouble;
class SCMatrixdouble;
class SymmSCMatrixdouble;
class DiagSCMatrixdouble;

class SCMatrixBlockIter;
class SCMatrixRectBlock;
class SCMatrixLTriBlock;
class SCMatrixDiagBlock;
class SCVectorSimpleBlock;

class RefSCMatrix;
class RefSymmSCMatrix;
DescribedClass_named_REF_dec(RefDCSCVector,SCVector);
//texi
//  The @code{RefSCVector} class is a smart pointer to an @code{SCVector}
//  specialization.  Valid indices range from @code{0} to @code{n-1}.
class RefSCVector: public RefDCSCVector {
    // standard overrides
  public:
    //texi Initializes the vector pointer to @code{0}.  The reference must
    // be initialized before it is used.
    RefSCVector();
    //texi Make this and @var{v} refer to the same @code{SCVector}.
    RefSCVector(const RefSCVector& v);
    //texi Make this refer to @var{v}.
    RefSCVector(SCVector *v);
    // don't allow automatic conversion from any reference to a
    // described class
    ~RefSCVector();
    //texi Make this refer to @var{v}.
    RefSCVector& operator=(SCVector* v);
    //texi Make this and @var{v} refer to the same @code{SCVector}.
    RefSCVector& operator=(const RefSCVector& v);

    // vector specific members
  public:
    //texi Create a vector with dimension @var{dim}.  The data values
    // are undefined.
    RefSCVector(const RefSCDimension& dim,const RefSCMatrixKit&);

    //texi Return an l-value that can be used to assign or retrieve an element.
    SCVectordouble operator()(int) const;
    SCVectordouble operator[](int) const;
    //texi Add two vectors.
    RefSCVector operator+(const RefSCVector&a) const;
    //texi Subtract two vectors.
    RefSCVector operator-(const RefSCVector&a) const;
    //texi Scale a vector.
    RefSCVector operator*(double) const;
    //texi Return the outer product between this and @var{v}.
    RefSCMatrix outer_product(const RefSCVector& v) const;
    //texi The outer product of this with itself is a symmetric matrix.
    RefSymmSCMatrix symmetric_outer_product() const;

    //texi These call the @code{SCMatrix} members of the same name
    // after checking for references to @code{0}.
    void set_element(int i,double val) const;
    void accumulate_element(int i,double val) const;
    double get_element(int) const;
    int n() const;
    RefSCDimension dim() const;
    RefSCMatrixKit kit() const;
    RefSCVector clone() const;
    RefSCVector copy() const;
    double maxabs() const;
    double scalar_product(const RefSCVector&) const;
    double dot(const RefSCVector&) const;
    void normalize() const;
    void randomize() const;
    void assign(const RefSCVector& v) const;
    void assign(double val) const;
    void assign(const double* v) const;
    void convert(double*) const;
    void scale(double val) const;
    void accumulate(const RefSCVector& v) const;
    void accumulate_product(const RefSymmSCMatrix&, const RefSCVector&);
    void accumulate_product(const RefSCMatrix&, const RefSCVector&);
    void element_op(const RefSCElementOp& op) const;
    void element_op(const RefSCElementOp2&,
                    const RefSCVector&) const;
    void element_op(const RefSCElementOp3&,
                    const RefSCVector&,
                    const RefSCVector&) const;
    void print(ostream&out) const;
    void print(const char*title=0, ostream&out=cout, int precision=10) const;
    void save(StateOut&);
    void restore(StateIn&);
};
RefSCVector operator*(double,const RefSCVector&);
ARRAY_dec(RefSCVector);
SET_dec(RefSCVector);

class RefSymmSCMatrix;
class RefDiagSCMatrix;
DescribedClass_named_REF_dec(RefDCSCMatrix,SCMatrix);
//texi
//  The @code{RefSCMatrix} class is a smart pointer to an @code{SCMatrix}
//  specialization.
class RefSCMatrix: public RefDCSCMatrix {
    // standard overrides
  public:
    //texi Initializes the matrix pointer to @var{0}.  The reference must
    // be initialized before it is used.
    RefSCMatrix();
    //texi Make this and @var{m} refer to the same @code{SCMatrix}.
    RefSCMatrix(const RefSCMatrix& m);
    //texi Make this refer to @var{m}.
     RefSCMatrix(SCMatrix* m);
    ~RefSCMatrix();
    //texi Make this refer to @var{m}.
    RefSCMatrix& operator=(SCMatrix* m);
    //texi Make this and @var{m} refer to the same matrix.
    RefSCMatrix& operator=(const RefSCMatrix& m);

    // matrix specific members
  public:
    //texi Create a vector with dimension @var{d1} by @var{d2}.
    // The data values are undefined.
    RefSCMatrix(const RefSCDimension& d1,const RefSCDimension& d2,
                const RefSCMatrixKit&);

    //texi Multiply this by a vector and return a vector.
    RefSCVector operator*(const RefSCVector&) const;

    //texi Multiply this by a matrix and return a matrix.
    RefSCMatrix operator*(const RefSCMatrix&) const;
    RefSCMatrix operator*(const RefSymmSCMatrix&) const;
    RefSCMatrix operator*(const RefDiagSCMatrix&) const;

    //texi Multiply this by a scalar and return the result.
    RefSCMatrix operator*(double) const;

    //texi Matrix addition and subtraction.
    RefSCMatrix operator+(const RefSCMatrix&) const;
    RefSCMatrix operator-(const RefSCMatrix&) const;

    //texi Return the transpose of this.
    RefSCMatrix t() const;
    //texi Return the inverse of this.
    RefSCMatrix i() const;
    //texi Return the generalized inverse of this.
    RefSCMatrix gi() const;

    //texi These call the @code{SCMatrix} members of the same name
    // after checking for references to @code{0}.
    RefSCMatrix clone() const;
    RefSCMatrix copy() const;

    RefSCMatrix get_subblock(int br, int er, int bc, int ec);
    void assign_subblock(const RefSCMatrix&, int br, int er, int bc, int ec,
                         int source_br = 0, int source_bc = 0);
    void accumulate_subblock(const RefSCMatrix&, int, int, int, int,
                             int source_br = 0, int source_bc = 0);
    RefSCVector get_row(int);
    RefSCVector get_column(int);
    void assign_row(const RefSCVector&, int);
    void assign_column(const RefSCVector&, int);
    void accumulate_row(const RefSCVector&, int);
    void accumulate_column(const RefSCVector&, int);

    void accumulate_outer_product(const RefSCVector&,const RefSCVector&) const;
    void accumulate_product(const RefSCMatrix&,const RefSCMatrix&) const;
    void assign(const RefSCMatrix&) const;
    void scale(double) const;
    void randomize() const;
    void assign(double) const;
    void assign(const double*) const;
    void assign(const double**) const;
    void convert(double*) const;
    void convert(double**) const;
    void accumulate(const RefSCMatrix&) const;
    void element_op(const RefSCElementOp&) const;
    void element_op(const RefSCElementOp2&,
                    const RefSCMatrix&) const;
    void element_op(const RefSCElementOp3&,
                    const RefSCMatrix&,
                    const RefSCMatrix&) const;
    int nrow() const;
    int ncol() const;
    RefSCDimension rowdim() const;
    RefSCDimension coldim() const;
    RefSCMatrixKit kit() const;
    void set_element(int,int,double) const;
    void accumulate_element(int,int,double) const;
    double get_element(int,int) const;
    void print(ostream&) const;
    void print(const char*title=0,ostream&out=cout, int =10) const;
    double trace() const;
    void save(StateOut&);
    void restore(StateIn&);

    //texi Compute the singular value decomposition.
    // @code{this} = @var{U} @var{sigma} @var{V}@code{.t()}.
    // The dimension of @var{sigma} is the smallest dimension of
    // @code{this}.  @var{U}, @var{V}, and @var{sigma} must already
    // have the correct dimensions and are overwritten.
    void svd(const RefSCMatrix &U,
             const RefDiagSCMatrix &sigma,
             const RefSCMatrix &V);
    //texi Solves @code{this} @var{x} = @var{v}.  Overwrites
    // @var{v} with @var{x}.
    double solve_lin(const RefSCVector& v) const;
    //texi Returns the determinant of the referenced matrix.
    double determ() const;
    //texi Assign and examine matrix elements.
    SCMatrixdouble operator()(int i,int j) const;
};
//texi Allow multiplication with a scalar on the left.
RefSCMatrix operator*(double,const RefSCMatrix&);

DescribedClass_named_REF_dec(RefDCSymmSCMatrix,SymmSCMatrix);
//texi
//  The @code{RefSymmSCMatrix} class is a smart pointer to an
//   @code{SCSymmSCMatrix} specialization.
class RefSymmSCMatrix: public RefDCSymmSCMatrix {
    // standard overrides
  public:
    //texi Initializes the matrix pointer to @var{0}.  The reference must
    // be initialized before it is used.
    RefSymmSCMatrix();
    //texi Make this and @var{m} refer to the same @code{SCMatrix}.
    RefSymmSCMatrix(const RefSymmSCMatrix& m);
    //texi Make this refer to @var{m}.
    RefSymmSCMatrix(SymmSCMatrix *m);
    ~RefSymmSCMatrix();
    //texi Make this refer to @var{m}.
    RefSymmSCMatrix& operator=(SymmSCMatrix* m);
    //texi Make this and @var{m} refer to the same matrix.
    RefSymmSCMatrix& operator=(const RefSymmSCMatrix& m);

    // matrix specific members
  public:
    //texi Create a vector with dimension @var{d} by @var{d}.
    // The data values are undefined.
    RefSymmSCMatrix(const RefSCDimension& d,const RefSCMatrixKit&);
    //texi Multiply this by a matrix and return a matrix.
    RefSCMatrix operator*(const RefSCMatrix&) const;
    //texi Multiply this by a vector and return a vector.
    RefSCVector operator*(const RefSCVector&a) const;
    RefSymmSCMatrix operator*(double) const;
    //texi Matrix addition and subtraction.
    RefSymmSCMatrix operator+(const RefSymmSCMatrix&) const;
    RefSymmSCMatrix operator-(const RefSymmSCMatrix&) const;
    //texi Return the inverse of this.
    RefSymmSCMatrix i() const;
    //texi Return the generalized inverse of this.
    RefSymmSCMatrix gi() const;
    //texi These call the @code{SCMatrix} members of the same name
    // after checking for references to @code{0}.
    RefSymmSCMatrix clone() const;
    RefSymmSCMatrix copy() const;
    void set_element(int,int,double) const;
    void accumulate_element(int,int,double) const;
    double get_element(int,int) const;

    RefSCMatrix get_subblock(int br, int er, int bc, int ec);
    RefSymmSCMatrix get_subblock(int br, int er);
    void assign_subblock(const RefSCMatrix&, int br, int er, int bc, int ec);
    void assign_subblock(const RefSymmSCMatrix&, int br, int er);
    void accumulate_subblock(const RefSCMatrix&, int, int, int, int);
    void accumulate_subblock(const RefSymmSCMatrix&, int, int);
    RefSCVector get_row(int);
    void assign_row(const RefSCVector&, int);
    void accumulate_row(const RefSCVector&, int);

    void accumulate_symmetric_outer_product(const RefSCVector&) const;
    double scalar_product(const RefSCVector&) const;
    void accumulate_symmetric_product(const RefSCMatrix&) const;
    void accumulate_symmetric_sum(const RefSCMatrix&) const;
    void accumulate_transform(const RefSCMatrix&,const RefSymmSCMatrix&) const;
    void accumulate_transform(const RefSCMatrix&,const RefDiagSCMatrix&) const;
    void accumulate_transform(const RefSymmSCMatrix&,const RefSymmSCMatrix&) const;
    void randomize() const;
    void assign(const RefSymmSCMatrix&) const;
    void scale(double) const;
    void assign(double) const;
    void assign(const double*) const;
    void assign(const double**) const;
    void convert(double*) const;
    void convert(double**) const;
    void accumulate(const RefSymmSCMatrix&) const;
    void element_op(const RefSCElementOp&) const;
    void element_op(const RefSCElementOp2&,
                    const RefSymmSCMatrix&) const;
    void element_op(const RefSCElementOp3&,
                    const RefSymmSCMatrix&,
                    const RefSymmSCMatrix&) const;
    double trace() const;
    int n() const;
    RefSCDimension dim() const;
    RefSCMatrixKit kit() const;
    void print(ostream&) const;
    void print(const char*title=0,ostream&out=cout, int =10) const;
    void save(StateOut&);
    void restore(StateIn&);

    //texi Solves @code{this} @var{x} = @var{v}.  Overwrites
    // @var{v} with @var{x}.
    double solve_lin(const RefSCVector&) const;
    //texi Returns the determinant of the referenced matrix.
    double determ() const;
    //texi Returns the eigenvalues of the reference matrix.
    RefDiagSCMatrix eigvals() const;
    //texi Returns the eigenvectors of the reference matrix.
    RefSCMatrix eigvecs() const;
    //texi Sets @var{eigvals} to the eigenvalues and @var{eigvecs}
    // to the eigenvalues and eigenvectors of the referenced matrix.
    void diagonalize(const RefDiagSCMatrix& eigvals,
                     const RefSCMatrix& eigvecs) const;
    //texi Assign and examine matrix elements.
    SymmSCMatrixdouble operator()(int i,int j) const;
};
//texi Allow multiplication with a scalar on the left.
RefSymmSCMatrix operator*(double,const RefSymmSCMatrix&);

DescribedClass_named_REF_dec(RefDCDiagSCMatrix,DiagSCMatrix);
//texi
//  The @code{RefDiagSCMatrix} class is a smart pointer to an
//  @code{DiagSCMatrix} specialization.
class RefDiagSCMatrix: public RefDCDiagSCMatrix {
    // standard overrides
  public:
    //texi Initializes the matrix pointer to @var{0}.  The reference must
    // be initialized before it is used.
    RefDiagSCMatrix();
    //texi Make this and @var{m} refer to the same @code{SCMatrix}.
    RefDiagSCMatrix(const RefDiagSCMatrix& m);
    //texi Make this refer to @var{m}.
    RefDiagSCMatrix(DiagSCMatrix *m);
    ~RefDiagSCMatrix();
    //texi Make this refer to @var{m}.
    RefDiagSCMatrix& operator=(DiagSCMatrix* m);
    //texi Make this and @var{m} refer to the same matrix.
    RefDiagSCMatrix& operator=(const RefDiagSCMatrix & m);

    // matrix specific members
  public:
    //texi Create a vector with dimension @var{d} by @var{d}.
    // The data values are undefined.
    RefDiagSCMatrix(const RefSCDimension&,const RefSCMatrixKit&);
    //texi Multiply this by a matrix and return a matrix.
    RefSCMatrix operator*(const RefSCMatrix&) const;
    RefDiagSCMatrix operator*(double) const;
    //texi Matrix addition and subtraction.
    RefDiagSCMatrix operator+(const RefDiagSCMatrix&) const;
    RefDiagSCMatrix operator-(const RefDiagSCMatrix&) const;
    //texi Return the inverse of this.
    RefDiagSCMatrix i() const;
    //texi Return the generalized inverse of this.
    RefDiagSCMatrix gi() const;
    //texi These call the @code{SCMatrix} members of the same name
    // after checking for references to @code{0}.
    RefDiagSCMatrix clone() const;
    RefDiagSCMatrix copy() const;
    void set_element(int,double) const;
    void accumulate_element(int,double) const;
    double get_element(int) const;
    void randomize() const;
    void assign(const RefDiagSCMatrix&) const;
    void scale(double) const;
    void assign(double) const;
    void assign(const double*) const;
    void convert(double*) const;
    void accumulate(const RefDiagSCMatrix&) const;
    void element_op(const RefSCElementOp&) const;
    void element_op(const RefSCElementOp2&,
                    const RefDiagSCMatrix&) const;
    void element_op(const RefSCElementOp3&,
                    const RefDiagSCMatrix&,
                    const RefDiagSCMatrix&) const;
    int n() const;
    RefSCDimension dim() const;
    RefSCMatrixKit kit() const;
    double trace() const;
    void print(ostream&) const;
    void print(const char*title=0,ostream&out=cout, int =10) const;
    void save(StateOut&);
    void restore(StateIn&);
    //texi Returns the determinant of the referenced matrix.
    double determ() const;
    //texi Assign and examine matrix elements.
    DiagSCMatrixdouble operator()(int i) const;
};
//texi Allow multiplication with a scalar on the left.
RefDiagSCMatrix operator*(double,const RefDiagSCMatrix&);

class SCVectordouble {
   friend class RefSCVector;
  private:
    RefSCVector vector;
    int i;
    
    SCVectordouble(SCVector*,int);
  public:
    SCVectordouble(const SCVectordouble&);
    ~SCVectordouble();
    double operator=(double a);
    double operator=(const SCVectordouble&);
    operator double();
    double val() const;
};

class SCMatrixdouble {
   friend class RefSCMatrix;
  private:
    RefSCMatrix matrix;
    int i;
    int j;
    
    SCMatrixdouble(SCMatrix*,int,int);
  public:
    SCMatrixdouble(const SCMatrixdouble&);
    ~SCMatrixdouble();
    double operator=(double a);
    double operator=(const SCMatrixdouble&);
    operator double();
    double val() const;
};

class SymmSCMatrixdouble {
   friend class RefSymmSCMatrix;
  private:
    RefSymmSCMatrix matrix;
    int i;
    int j;
    
    SymmSCMatrixdouble(SymmSCMatrix*,int,int);
  public:
    SymmSCMatrixdouble(const SCMatrixdouble&);
    ~SymmSCMatrixdouble();
    double operator=(double a);
    double operator=(const SymmSCMatrixdouble&);
    operator double();
    double val() const;
};

class DiagSCMatrixdouble {
   friend class RefDiagSCMatrix;
  private:
    RefDiagSCMatrix matrix;
    int i;
    int j;
    
    DiagSCMatrixdouble(DiagSCMatrix*,int,int);
  public:
    DiagSCMatrixdouble(const SCMatrixdouble&);
    ~DiagSCMatrixdouble();
    double operator=(double a);
    double operator=(const DiagSCMatrixdouble&);
    operator double();
    double val() const;
};

#ifdef INLINE_FUNCTIONS
#include <math/scmat/matrix_i.h>
#endif

#endif
