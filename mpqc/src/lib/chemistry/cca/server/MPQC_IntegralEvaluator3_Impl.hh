// 
// File:          MPQC_IntegralEvaluator3_Impl.hh
// Symbol:        MPQC.IntegralEvaluator3-v0.2
// Symbol Type:   class
// Babel Version: 0.10.12
// Description:   Server-side implementation for MPQC.IntegralEvaluator3
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.12
// 

#ifndef included_MPQC_IntegralEvaluator3_Impl_hh
#define included_MPQC_IntegralEvaluator3_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_IntegralEvaluator3_IOR_h
#include "MPQC_IntegralEvaluator3_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_QC_GaussianBasis_CompositeIntegralDescr_hh
#include "Chemistry_QC_GaussianBasis_CompositeIntegralDescr.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_DerivCenters_hh
#include "Chemistry_QC_GaussianBasis_DerivCenters.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_IntegralDescr_hh
#include "Chemistry_QC_GaussianBasis_IntegralDescr.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_Molecular_hh
#include "Chemistry_QC_GaussianBasis_Molecular.hh"
#endif
#ifndef included_MPQC_IntegralEvaluator3_hh
#include "MPQC_IntegralEvaluator3.hh"
#endif
#ifndef included_sidl_BaseException_hh
#include "sidl_BaseException.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator3._includes)
#include "integral_evaluator.h"
using namespace sc;
using namespace Chemistry::QC::GaussianBasis;
using namespace MpqcCca;
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator3._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.IntegralEvaluator3" (version 0.2)
   */
  class IntegralEvaluator3_impl
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator3._inherits)

  /** IntegralEvaluator3_impl implements a class interface for
      supplying 3-center molecular integrals.

      This is an implementation of a SIDL interface.
      The stub code is generated by the Babel tool.  Do not make
      modifications outside of splicer blocks, as these will be lost.
      This is a server implementation for a Babel class, the Babel
      client code is provided by the cca-chem-generic package.
   */

  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator3._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    IntegralEvaluator3 self;

    // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator3._implementation)
 
    vector< Molecular > basis_sets_;
    IntegralEvaluator< TwoBodyThreeCenterInt, 
		       twobody_threecenter_computer > eval_;
    twobody_threecenter_computer computer_;

    // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator3._implementation)

  private:
    // private default constructor (required)
    IntegralEvaluator3_impl() 
    {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    IntegralEvaluator3_impl( struct MPQC_IntegralEvaluator3__object * s ) : 
      self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~IntegralEvaluator3_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

    // static class initializer
    static void _load();

  public:

    /**
     * user defined non-static method.
     */
    void
    add_evaluator (
      /* in */ void* eval,
      /* in */ ::Chemistry::QC::GaussianBasis::IntegralDescr desc
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_basis (
      /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs1,
      /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs2,
      /* in */ ::Chemistry::QC::GaussianBasis::Molecular bs3
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_reorder (
      /* in */ int32_t reorder
    )
    throw () 
    ;


    /**
     * Get buffer pointer for given type.
     * @return Buffer pointer. 
     */
    void*
    get_buffer (
      /* in */ ::Chemistry::QC::GaussianBasis::IntegralDescr desc
    )
    throw ( 
      ::sidl::BaseException
    );

    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::DerivCenters
    get_deriv_centers() throw ( 
      ::sidl::BaseException
    );
    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::CompositeIntegralDescr
    get_descriptor() throw ( 
      ::sidl::BaseException
    );

    /**
     * Compute a shell triplet of integrals.
     * @param shellnum1 Gaussian shell number 1.
     * @param shellnum2 Gaussian shell number 2.
     * @param shellnum3 Gaussian shell number 3.
     * @param deriv_level Derivative level. 
     */
    void
    compute (
      /* in */ int64_t shellnum1,
      /* in */ int64_t shellnum2,
      /* in */ int64_t shellnum3
    )
    throw ( 
      ::sidl::BaseException
    );


    /**
     * Compute a shell triplet of integrals and return as a borrowed
     * sidl array.
     * @param shellnum1 Gaussian shell number 1.
     * @param shellnum2 Gaussian shell number 2.
     * @param shellnum3 Gaussian shell number 3.
     * @return Borrowed sidl array buffer. 
     */
    ::sidl::array<double>
    compute_array (
      /* in */ int64_t shellnum1,
      /* in */ int64_t shellnum2,
      /* in */ int64_t shellnum3
    )
    throw ( 
      ::sidl::BaseException
    );

  };  // end class IntegralEvaluator3_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluator3._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluator3._misc)

#endif
