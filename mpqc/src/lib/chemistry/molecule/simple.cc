
/* simple.cc -- implementation of the simple internal coordinate classes
 *
 *      THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
 *      "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
 *      AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
 *      CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
 *      PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
 *      RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.
 *
 *  Author:
 *      E. T. Seidl
 *      Bldg. 12A, Rm. 2033
 *      Computer Systems Laboratory
 *      Division of Computer Research and Technology
 *      National Institutes of Health
 *      Bethesda, Maryland 20892
 *      Internet: seidl@alw.nih.gov
 *      February, 1993
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>

#if defined(SGI) && !defined(__GNUC__)
#include <bstring.h>
#endif

#include "simple.h"
#include "chemelem.h"
#include "localdef.h"

#include <math/topology/bitarray.h>

//////////////////////////////////////////////////////////////////////

SavableState_REF_def(SimpleCo);

#define CLASSNAME SimpleCo
#define PARENTS public IntCoor
#include <util/state/statei.h>
#include <util/class/classi.h>
void *
SimpleCo::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = IntCoor::_castdown(cd);
  return do_castdowns(casts,cd);
}

SimpleCo::SimpleCo():
  natoms_(0),
  atoms(0)
{
}

SimpleCo::SimpleCo(int na, const char *re) :
  IntCoor(re),
  natoms_(na), atoms(0)
{
  atoms=new int[na]; memset(atoms,'\0',sizeof(int)*na);
}

SimpleCo::SimpleCo(const RefKeyVal&kv,int na) :
  IntCoor(0),
  natoms_(na), atoms(0)
{
  atoms=new int[na];
  memset(atoms,'\0',sizeof(int)*na);

  label_=kv->pcharvalue(0);
  for (int i=0; i<na; i++) {
      atoms[i]=kv->intvalue(i+1);
      if (kv->error() != KeyVal::OK) {
          fprintf(stderr,"%s::%s(const RefKeyVal&): missing an atom\n",
                  class_name(),class_name());
          abort();
        }
    }
}

SimpleCo::~SimpleCo()
{
  if(atoms) delete[] atoms; atoms=0;
  natoms_=0;
}

void
SimpleCo::save_data_state(StateOut& s)
{
  IntCoor::save_data_state(s);
  s.put(natoms_);
  s.put(atoms,natoms_);
}

SimpleCo::SimpleCo(StateIn& si):
  IntCoor(si)
{
  si.get(natoms_);
  si.get(atoms);
}

int
SimpleCo::natoms() const
{
  return natoms_;
}

int
SimpleCo::operator[](int i) const
{
  return atoms[i];
}

int
SimpleCo::operator!=(SimpleCo&u)
{
  return !(*this==u);
}

int
SimpleCo::operator==(SimpleCo& sc)
{
  if(label_ && !sc.label_ || !label_ && sc.label_) return 0;
  if(label_ && strcmp(label_,sc.label_)) return 0;

  if(atoms && !sc.atoms || !atoms && sc.atoms) return 0;
  if(atoms)
    for(int i=0; i < natoms_; i++) if (atoms[i]!=sc.atoms[i]) return 0;

  return 1;
}

double
SimpleCo::force_constant(RefMolecule&mol)
{
  return calc_force_con(*mol);
}

// this updates the values before it computes the bmatrix,
// which is not quite what I wanted--but close enough
void
SimpleCo::bmat(RefMolecule&mol,RefSCVector&bmat,double coef)
{
  int i;
  int n = bmat.dim().n();

  double* v = new double[n];
  for (i=0; i<n; i++) v[i] = bmat(i);

  calc_intco(*mol,v,coef);

  for (i=0; i<n; i++) {
      bmat(i) = v[i];
    }
  
  delete[] v;
}

void
SimpleCo::update_value(RefMolecule&mol)
{
  calc_intco(*mol);
}

#ifndef __GNUC__
void
SimpleCo::print()
{
  print(0);
}
#endif

void
SimpleCo::print(RefMolecule mol, SCostream& os)
{
  os.setf(ios::fixed,ios::floatfield);
  os.precision(10);
  os.setf(ios::left,ios::adjustfield);
  os.width(10);

  os.width(5);
  os.indent() << ctype()
              << " ";
  os.width(10);
  os          << label()
              << " ";
  os.width(14);
  os.setf(ios::right,ios::adjustfield);
  os          << preferred_value();

  for (int i=0; i<natoms(); i++) {
      os.width(2);
      os << " " << atoms[i];
    }

  if (mol.nonnull()) {
      char *separator = " ";
      os << "  ";
      for (i=0; i<(4-natoms()); i++) {
          os << "   ";
        }
      for (int i=0; i<natoms(); i++) {
          os << separator << mol->atom(atoms[i]-1).element().symbol();
          separator = "-";
        }
    }

  os << endl;
  
}

// this doesn't catch all cases, it would be best for each subclass
// to override this
int
SimpleCo::equivalent(RefIntCoor&c)
{
  if (class_desc() != c->class_desc()) {
      return 0;
    }
  SimpleCo* sc = SimpleCo::castdown(c.pointer());
  if (natoms_ != sc->natoms_) return 0; // this should never be the case
  for (int i=0; i<natoms_; i++) {
      if (atoms[i] != sc->atoms[i]) return 0;
    }
  return 1;
}
