// 
// File:          MPQC_Chemistry_QC_ModelFactory_Impl.hh
// Symbol:        MPQC.Chemistry_QC_ModelFactory-v0.2
// Symbol Type:   class
// Babel Version: 0.9.8
// Description:   Server-side implementation for MPQC.Chemistry_QC_ModelFactory
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.9.8
// 

#ifndef included_MPQC_Chemistry_QC_ModelFactory_Impl_hh
#define included_MPQC_Chemistry_QC_ModelFactory_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_Chemistry_QC_ModelFactory_IOR_h
#include "MPQC_Chemistry_QC_ModelFactory_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_Molecule_hh
#include "Chemistry_Molecule.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_IntegralEvaluatorFactory_hh
#include "Chemistry_QC_GaussianBasis_IntegralEvaluatorFactory.hh"
#endif
#ifndef included_Chemistry_QC_Model_hh
#include "Chemistry_QC_Model.hh"
#endif
#ifndef included_MPQC_Chemistry_QC_ModelFactory_hh
#include "MPQC_Chemistry_QC_ModelFactory.hh"
#endif
#ifndef included_gov_cca_CCAException_hh
#include "gov_cca_CCAException.hh"
#endif
#ifndef included_gov_cca_Services_hh
#include "gov_cca_Services.hh"
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


// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_QC_ModelFactory._includes)

#include <string>
#include "Chemistry_MoleculeFactory.hh"
#include <util/group/message.h>
#include <util/group/memory.h>
#include <util/group/mstate.h>
#include <util/group/thread.h>
#include <util/group/pregtime.h>
#include <util/group/messmpi.h>
#include <chemistry/qc/intcca/intcca.h>
#include <chemistry/qc/basis/integral.h>
#include "cca.h"
#include "dc/babel/babel-cca/server/ccaffeine_TypeMap.hh"
#include "dc/babel/babel-cca/server/ccaffeine_ports_PortTranslator.hh"
#include "util/IO.h"
#include "jc++/jc++.h"
#include "jc++/util/jc++util.h"
#include "parameters/parametersStar.h"
#include "port/portInterfaces.h"
#include "port/supportInterfaces.h"

// DO-NOT-DELETE splicer.end(MPQC.Chemistry_QC_ModelFactory._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.Chemistry_QC_ModelFactory" (version 0.2)
   */
  class Chemistry_QC_ModelFactory_impl
  // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_QC_ModelFactory._inherits)
  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.Chemistry_QC_ModelFactory._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    Chemistry_QC_ModelFactory self;

    // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_QC_ModelFactory._implementation)
      std::string theory_;
      std::string basis_;
      std::string molecule_filename_;
      StringParameter *theory_param_, *basis_param_, *molecule_filename_param_, 
                      *keyval_filename_param_, *integral_buffer_param_;

      gov::cca::Services services_;

      Chemistry::MoleculeFactory molecule_factory_;
      Chemistry::Molecule molecule_;
      Chemistry::QC::GaussianBasis::IntegralEvaluatorFactory eval_factory_;

      sc::Ref<sc::MessageGrp> grp_;
      sc::Ref<sc::ThreadGrp> thread_;
      sc::Ref<sc::MemoryGrp> memory_;

      sc::Ref<sc::IntegralCCA> intcca_;

      ConfigurableParameterPort* 
        setup_parameters(ConfigurableParameterFactory*);

    // DO-NOT-DELETE splicer.end(MPQC.Chemistry_QC_ModelFactory._implementation)

  private:
    // private default constructor (required)
    Chemistry_QC_ModelFactory_impl() {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    Chemistry_QC_ModelFactory_impl( struct 
      MPQC_Chemistry_QC_ModelFactory__object * s ) : self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~Chemistry_QC_ModelFactory_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

  public:


    /**
     * Starts up a component presence in the calling framework.
     * @param Svc the component instance's handle on the framework world.
     * Contracts concerning Svc and setServices:
     * 
     * The component interaction with the CCA framework
     * and Ports begins on the call to setServices by the framework.
     * 
     * This function is called exactly once for each instance created
     * by the framework.
     * 
     * The argument Svc will never be nil/null.
     * 
     * Those uses ports which are automatically connected by the framework
     * (so-called service-ports) may be obtained via getPort during
     * setServices.
     */
    void
    setServices (
      /*in*/ ::gov::cca::Services services
    )
    throw ( 
      ::gov::cca::CCAException
    );


    /**
     * Set the theory name for Model's created with get_model.
     * @param theory A string giving the name of the theory, for example, B3LYP.
     */
    void
    set_theory (
      /*in*/ const ::std::string& theory
    )
    throw () 
    ;


    /**
     * Set the basis set name for Model's created with get_model.
     * @param basis The basis set name to use, for example, aug-cc-pVDZ.
     */
    void
    set_basis (
      /*in*/ const ::std::string& basis
    )
    throw () 
    ;


    /**
     * Set the Molecule to use for Model's created with get_model.
     * @param molecule An object of type Molecule.
     */
    void
    set_molecule (
      /*in*/ ::Chemistry::Molecule molecule
    )
    throw () 
    ;


    /**
     * Set the object to use to compute integrals for Model's created with get_model.
     * 
     * @param intfact An object of type GaussianBasis.IntegralEvaluatorFactory.
     */
    void
    set_integral_factory (
      /*in*/ ::Chemistry::QC::GaussianBasis::IntegralEvaluatorFactory intfact
    )
    throw () 
    ;


    /**
     * Returns a newly created Model.  Before get_model can be called, set_theory,
     * set_basis, and set_molecule must be called.
     * @return The new Model instance.
     */
    ::Chemistry::QC::Model
    get_model() throw ( 
      ::sidl::BaseException
    );

    /**
     * This can be called when this Model object is no longer needed.  No other
     * members may be called after finalize. 
     */
    int32_t
    finalize() throw () 
    ;
  };  // end class Chemistry_QC_ModelFactory_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_QC_ModelFactory._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.Chemistry_QC_ModelFactory._misc)

#endif
