// 
// File:          MPQC_IntegralEvaluator4_Impl.cc
// Symbol:        MPQC.IntegralEvaluator4-v0.2
// Symbol Type:   class
// Babel Version: 0.10.12
// Description:   Server-side implementation for MPQC.IntegralEvaluator4
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.12
// 
#include "MPQC_IntegralEvaluator4_Impl.hh"

// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4._includes)

sc::Ref<sc::GaussianBasisSet>
basis_cca_to_sc( Chemistry::QC::GaussianBasis::Molecular& );

// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4._includes)

// user-defined constructor.
void MPQC::IntegralEvaluator4_impl::_ctor() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4._ctor)
  reorder_ = false;
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4._ctor)
}

// user-defined destructor.
void MPQC::IntegralEvaluator4_impl::_dtor() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4._dtor)
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4._dtor)
}

// static class initializer.
void MPQC::IntegralEvaluator4_impl::_load() {
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4._load)
  // guaranteed to be called at most once before any other method in this class
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4._load)
}

// user-defined static methods: (none)

// user-defined non-static methods:
/**
 * Method:  add_evaluator[]
 */
void
MPQC::IntegralEvaluator4_impl::add_evaluator (
  /* in */ void* eval,
  /* in */ ::Chemistry::QC::GaussianBasis::IntegralDescr desc ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.add_evaluator)

  eval_.add_evaluator(eval,desc);

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.add_evaluator)
}

/**
 * Method:  set_basis[]
 */
void
MPQC::IntegralEvaluator4_impl::set_basis (
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs3,
  /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs4 ) 
throw () 
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.set_basis)

  bs1_ = basis_cca_to_sc( bs1 );
  if( bs2.isSame(bs1) )
    bs2_ = bs1_;
  else
    bs2_ = basis_cca_to_sc( bs2 );
  if( bs3.isSame(bs2) )
    bs3_ = bs2_;
  else
    bs3_ = basis_cca_to_sc( bs3 );
  if( bs4.isSame(bs3) )
    bs4_ = bs3_;
  else
    bs4_ = basis_cca_to_sc( bs4 );

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.set_basis)
}

/**
 * Method:  init_reorder[]
 */
void
MPQC::IntegralEvaluator4_impl::init_reorder ()
throw () 

{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.init_reorder)

  reorder_ = true;
  reorder_engine_.init( 4, bs1_, bs2_, bs3_, bs4_ );
  CompositeIntegralDescr desc = eval_.get_descriptor();
  for( int i=0; i < desc.get_n_descr(); ++i) {
    IntegralDescr idesc = desc.get_descr(i);
    reorder_engine_.add_buffer( eval_.get_buffer( idesc ), idesc );
  }

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.init_reorder)
}

/**
 * Get buffer pointer for given type.
 * @return Buffer pointer. 
 */
void*
MPQC::IntegralEvaluator4_impl::get_buffer (
  /* in */ ::Chemistry::QC::GaussianBasis::IntegralDescr desc ) 
throw ( 
  ::sidl::BaseException
){
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.get_buffer)

  return eval_.get_buffer( desc );

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.get_buffer)
}

/**
 * Method:  get_deriv_centers[]
 */
::Chemistry::QC::GaussianBasis::DerivCenters
MPQC::IntegralEvaluator4_impl::get_deriv_centers ()
throw ( 
  ::sidl::BaseException
)
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.get_deriv_centers)

  return eval_.get_deriv_centers();

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.get_deriv_centers)
}

/**
 * Method:  get_descriptor[]
 */
::Chemistry::QC::GaussianBasis::CompositeIntegralDescr
MPQC::IntegralEvaluator4_impl::get_descriptor ()
throw ( 
  ::sidl::BaseException
)
{
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.get_descriptor)

  return eval_.get_descriptor();
  
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.get_descriptor)
}

/**
 * Compute a shell quartet of integrals.
 * @param shellnum1 Gaussian shell number 1.
 * @param shellnum2 Gaussian shell number 2.
 * @param shellnum3 Gaussian shell number 3.
 * @param shellnum4 Gaussian shell number 4.
 * @param deriv_level Derivative level. 
 */
void
MPQC::IntegralEvaluator4_impl::compute (
  /* in */ int64_t shellnum1,
  /* in */ int64_t shellnum2,
  /* in */ int64_t shellnum3,
  /* in */ int64_t shellnum4 ) 
throw ( 
  ::sidl::BaseException
){
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.compute)

  computer_.set_shells( shellnum1, shellnum2, shellnum3, shellnum4 );
  eval_.compute( &computer_ );
  if( reorder_ )
    reorder_engine_.do_it( shellnum1, shellnum2, shellnum3, shellnum4 );

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.compute)
}

/**
 * Compute a shell quartet of integrals and return as a borrowed
 * sidl array.
 * @param shellnum1 Gaussian shell number 1.
 * @param shellnum2 Gaussian shell number 2.
 * @param shellnum3 Gaussian shell number 3.
 * @param shellnum4 Gaussian shell number 4.
 * @return Borrowed sidl array buffer. 
 */
::sidl::array<double>
MPQC::IntegralEvaluator4_impl::compute_array (
  /* in */ int64_t shellnum1,
  /* in */ int64_t shellnum2,
  /* in */ int64_t shellnum3,
  /* in */ int64_t shellnum4 ) 
throw ( 
  ::sidl::BaseException
){
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.compute_array)

  computer_.set_shells( shellnum1, shellnum2, shellnum3, shellnum4 );
  eval_.compute_array( &computer_ );

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.compute_array)
}

/**
 * Compute integral bounds.
 * @param shellnum1 Gaussian shell number 1.
 * @param shellnum2 Gaussian shell number 2.
 * @param shellnum3 Gaussian shell number 3.
 * @param shellnum4 Gaussian shell number 4. 
 */
double
MPQC::IntegralEvaluator4_impl::compute_bounds (
  /* in */ int64_t shellnum1,
  /* in */ int64_t shellnum2,
  /* in */ int64_t shellnum3,
  /* in */ int64_t shellnum4 ) 
throw ( 
  ::sidl::BaseException
){
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4.compute_bounds)

  computer_.set_shells( shellnum1, shellnum2, shellnum3, shellnum4 );
  return eval_.compute_bounds( &computer_ );

  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4.compute_bounds)
}


// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator4._misc)
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator4._misc)
