// 
// File:          MPQC_Physics_Units_Impl.hh
// Symbol:        MPQC.Physics_Units-v0.2
// Symbol Type:   class
// Babel Version: 0.10.2
// Description:   Server-side implementation for MPQC.Physics_Units
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.2
// 

#ifndef included_MPQC_Physics_Units_Impl_hh
#define included_MPQC_Physics_Units_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_Physics_Units_IOR_h
#include "MPQC_Physics_Units_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_MPQC_Physics_Units_hh
#include "MPQC_Physics_Units.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.Physics_Units._includes)
#include <util/misc/units.h>
// DO-NOT-DELETE splicer.end(MPQC.Physics_Units._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.Physics_Units" (version 0.2)
   */
  class Physics_Units_impl
  // DO-NOT-DELETE splicer.begin(MPQC.Physics_Units._inherits)

  /** Physics_Units_impl implements a class interface for units.

      This is an implementation of a SIDL interface.
      The stub code is generated by the Babel tool.  Do not make
      modifications outside of splicer blocks, as these will be lost.
      This is a server implementation for a Babel class, the Babel
      client code is provided by the cca-chem package.
   */

  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.Physics_Units._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    Physics_Units self;

    // DO-NOT-DELETE splicer.begin(MPQC.Physics_Units._implementation)
      sc::Ref<sc::Units> units;
    public:
      void set_units(const sc::Ref<sc::Units> &);
    // DO-NOT-DELETE splicer.end(MPQC.Physics_Units._implementation)

  private:
    // private default constructor (required)
    Physics_Units_impl() 
    {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    Physics_Units_impl( struct MPQC_Physics_Units__object * s ) : self(s,
      true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~Physics_Units_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

    // static class initializer
    static void _load();

  public:


    /**
     * Initializes the units as a human readable string
     * options are "angstroms" or "bohr" 
     */
    void
    initialize (
      /* in */ const ::std::string& unitname
    )
    throw () 
    ;


    /**
     * Returns the units as a human readable string. 
     */
    ::std::string
    get_unit_name() throw () 
    ;

    /**
     * Converts from self's units to the given unit name. 
     */
    double
    convert_to (
      /* in */ const ::std::string& unitname
    )
    throw () 
    ;


    /**
     * Converts to self's units from the given unit name. 
     */
    double
    convert_from (
      /* in */ const ::std::string& unitname
    )
    throw () 
    ;

  };  // end class Physics_Units_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.Physics_Units._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.Physics_Units._misc)

#endif
