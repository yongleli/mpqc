// 
// File:          MPQC_ChemistryOpt_CoordinateModel_Impl.hh
// Symbol:        MPQC.ChemistryOpt_CoordinateModel-v0.2
// Symbol Type:   class
// Babel Version: 0.9.8
// Description:   Server-side implementation for MPQC.ChemistryOpt_CoordinateModel
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.9.8
// 

#ifndef included_MPQC_ChemistryOpt_CoordinateModel_Impl_hh
#define included_MPQC_ChemistryOpt_CoordinateModel_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_ChemistryOpt_CoordinateModel_IOR_h
#include "MPQC_ChemistryOpt_CoordinateModel_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_QC_Model_hh
#include "Chemistry_QC_Model.hh"
#endif
#ifndef included_MPQC_ChemistryOpt_CoordinateModel_hh
#include "MPQC_ChemistryOpt_CoordinateModel.hh"
#endif
#ifndef included_gov_cca_CCAException_hh
#include "gov_cca_CCAException.hh"
#endif
#ifndef included_gov_cca_Services_hh
#include "gov_cca_Services.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.ChemistryOpt_CoordinateModel._includes)
#include <Chemistry_MoleculeViewer.hh>
#include <Chemistry_QC_ModelFactory.hh>
#include <chemistry/molecule/coor.h>
#include "CoordinateModel.h"
#include "Chemistry_Chemistry_Molecule.hh"
#include "cca.h"
#include "dc/babel/babel-cca/server/ccaffeine_TypeMap.hh"
#include "dc/babel/babel-cca/server/ccaffeine_ports_PortTranslator.hh"
#include "util/IO.h"
#include "jc++/jc++.h"
#include "jc++/util/jc++util.h"
#include "parameters/parametersStar.h"
#include "port/portInterfaces.h"
#include "port/supportInterfaces.h"
// DO-NOT-DELETE splicer.end(MPQC.ChemistryOpt_CoordinateModel._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.ChemistryOpt_CoordinateModel" (version 0.2)
   */
  class ChemistryOpt_CoordinateModel_impl
  // DO-NOT-DELETE splicer.begin(MPQC.ChemistryOpt_CoordinateModel._inherits)
  : public CcaChemGeneric::CoordinateModel
  // DO-NOT-DELETE splicer.end(MPQC.ChemistryOpt_CoordinateModel._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    ChemistryOpt_CoordinateModel self;

    // DO-NOT-DELETE splicer.begin(MPQC.ChemistryOpt_CoordinateModel._implementation)
    gov::cca::Services services_;
    CcaChemGeneric::CoordinateModel genericModel_;
    Chemistry::QC::Model model_;
    Chemistry::Chemistry_Molecule molecule_;
    sc::Ref<sc::MolecularCoor> scCoor_;
    sc::Ref<sc::Molecule> scMol_;
    sc::Ref<sc::SCMatrixKit> kit_;
    sc::Ref<sc::SCMatrixKit> rkit_;
    sc::RefSymmSCMatrix ihess_;
    DoubleParameter *grad_rms_, *grad_max_, *disp_rms_, *disp_max_;
    BoolParameter *multiple_guess_h_, *use_current_geom_;
    StringParameter *coordinates_;
    StringParameter *extra_bonds_;
    double convFrom_;
    bool have_guess_h_;
    enum {cart,symm,redund};
    int coorType_;
    int numCoor_;
    int natom3_;

    ConfigurableParameterPort* 
    setup_parameters(ConfigurableParameterFactory *);
    
    void draw();
    // DO-NOT-DELETE splicer.end(MPQC.ChemistryOpt_CoordinateModel._implementation)

  private:
    // private default constructor (required)
    ChemistryOpt_CoordinateModel_impl() {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    ChemistryOpt_CoordinateModel_impl( struct 
      MPQC_ChemistryOpt_CoordinateModel__object * s ) : self(s,
      true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~ChemistryOpt_CoordinateModel_impl() { _dtor(); }

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
     * Registers and gets ports, and requests Model object(s) from the 
     * ModelFactory component(s). This must be the first method called 
     * following instantiation.
     */
    int32_t
    initialize() throw () 
    ;

    /**
     * Releases and unregisters ports.  This should be called when the
     * CoordinateModel object is no longer needed.
     */
    int32_t
    finalize() throw () 
    ;

    /**
     * Sets the contained chemistry Model object (currently unused as the
     * chemistry Model object is normally obtained from a ModelFactory 
     * during initialization).
     * @param model The chemistry model object.
     */
    void
    set_model (
      /*in*/ ::Chemistry::QC::Model model
    )
    throw () 
    ;


    /**
     * Returns the contained chemistry Model object.
     * @return The chemistry Model object.
     */
    ::Chemistry::QC::Model
    get_model() throw () 
    ;

    /**
     * Returns the number of coordinates.
     * @return The number of coordinates. 
     */
    int32_t
    get_n_coor() throw () 
    ;

    /**
     * Returns the array of (cartesian or internal) coordinates which are 
     * being  optimized.
     * @return The array of coordinates which are being optimized.
     */
    ::sidl::array<double>
    get_coor() throw () 
    ;

    /**
     * Returns the energy of the currently contained model with the values
     * of the optimization coordinates given in x.  This requires
     * that the CoordinateModel updates the cartesian coordinates of a 
     * contained Molecule object (possibly requiring transformation) and set 
     * this Molecule object on a contained Model object, prior to calling
     * get_energy() on the Model object.
     * @param x The optimization coordinate values.
     * @return The energy of the chemistry model at x.
     */
    double
    get_energy (
      /*in*/ ::sidl::array<double> x
    )
    throw () 
    ;


    /**
     * Returns the energy gradient of the currently contained model with 
     * the values of the optimization coordinates given in x.  This requires
     * that the CoordinateModel updates the cartesian coordinates of a
     * contained Molecule object (possibly requiring transformation) and set
     * this Molecule object on a contained Model object, prior to calling
     * get_gradient() on the Model object.  If the optimization coordinate
     * system is not cartesian, the gradient is transformed.
     * @param x The optimization coordinate values.
     * @return The energy gradient of the chemistry model at x.
     */
    ::sidl::array<double>
    get_gradient (
      /*in*/ ::sidl::array<double> x
    )
    throw () 
    ;


    /**
     * Returns the energy Hessian of the currently contained model with
     * the values of the optimization coordinates given in x.  This requires
     * that the CoordinateModel updates the cartesian coordinates of a
     * contained Molecule object (possibly requiring transformation) and set
     * this Molecule object on a contained Model object, prior to calling
     * get_hessian() on the Model object.  If the optimization coordinate
     * system is not cartesian, the Hessian is transformed.
     * @param x The optimization coordinate values.
     * @return The energy Hessian of the chemistry model at x.
     */
    ::sidl::array<double>
    get_hessian (
      /*in*/ ::sidl::array<double> x
    )
    throw () 
    ;


    /**
     * Sets f and g to the energy and energy gradient, respectively,
     * of the chemistry model at x.  This is similar to calling
     * get_energy() and get_gradient() separately, but set_molecule()
     * must be called on the Model object only once.  This is necessary
     * for some model implementations, as a second molecule update
     * would invalidate results from an energy computation.  An alternative
     * would be to always return the energy as well when get_gradient() is 
     * called.
     * @param x The optimization coordinate values.
     * @param f Variable that energy will be assigned to.
     * @param g Array that the gradient will be assigned to.
     */
    void
    get_energy_and_gradient (
      /*in*/ ::sidl::array<double> x,
      /*out*/ double& f,
      /*in*/ ::sidl::array<double> g
    )
    throw () 
    ;


    /**
     * Returns the product of the guess hessian inverse and an effective
     * gradient.  Probably unique to TAO's limited memory variable metric
     * algorithm, which uses this method to accomodate dense guess hessians.
     * "first_geom_ptr" provides the Cartesian coordinates for which the
     * guess Hessian should be computed (first_geom_ptr=0 for current
     * geometry).
     * @param effective_grad An effective gradient.
     * @param effective_step Array that effective step is assigned to.
     * @param first_geom     Pointer to array of Cartesians 
     */
    void
    guess_hessian_solve (
      /*in*/ ::sidl::array<double> effective_grad,
      /*in*/ ::sidl::array<double> effective_step,
      /*in*/ void* first_geom
    )
    throw () 
    ;


    /**
     * Determines if the optimization has converged, flag is set to 1
     * if convergence has been achieved and 0 otherwise.
     * @param flag Variable that convergence value is assigned to.
     */
    void
    checkConvergence (
      /*inout*/ int32_t& flag
    )
    throw () 
    ;


    /**
     * For visualization, possibly unused (?).  CoordinateModel objects
     * may callback to viewers that implement the Chemistry.MoleculeViewer 
     * interface, such as the cca-chem python GUI, making this method 
     * unnecessary.
     */
    void
    monitor() throw () 
    ;
  };  // end class ChemistryOpt_CoordinateModel_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.ChemistryOpt_CoordinateModel._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.ChemistryOpt_CoordinateModel._misc)

#endif
