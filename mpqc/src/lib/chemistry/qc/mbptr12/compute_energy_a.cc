//
// compute_energy_a.cc
//
// Copyright (C) 2003 Edward Valeev
//
// Author: Edward Valeev <edward.valeev@chemistry.gatech.edu>
// Maintainer: EV
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#include <stdexcept>
#include <scconfig.h>
#include <util/misc/math.h>
#include <util/misc/formio.h>
#include <util/misc/timer.h>
#include <math/scmat/abstract.h>
#include <chemistry/qc/mbptr12/mbptr12.h>
#include <chemistry/qc/mbptr12/mp2r12_energy.h>
#include <chemistry/qc/mbptr12/transform_factory.h>

using namespace std;
using namespace sc;

void
MBPT2_R12::compute_energy_a_()
{
  tim_enter("mp2-f12 energy");

  if (r12eval_.null()) {
    Ref<R12IntEvalInfo> r12info = new R12IntEvalInfo(this);
    r12info->set_dynamic(dynamic_);
    r12info->set_print_percent(print_percent_);
    r12info->set_memory(mem_alloc);
    r12eval_ = new R12IntEval(r12info,corrfactor_,gbc_,ebc_,abs_method_,stdapprox_);
    r12eval_->include_mp1(include_mp1_);
    r12eval_->set_debug(debug_);
  }
  // This will actually compute the intermediates
  r12eval_->compute();

  double etotal = 0.0;
  
  // Now we can compute and print pair energies
  tim_enter("mp2-f12/a pair energies");
  if (r12a_energy_.null())
    r12a_energy_ = new MP2R12Energy(r12eval_,LinearR12::StdApprox_A,debug_);
  r12a_energy_->print_pair_energies(spinadapted_);
  etotal = r12a_energy_->energy();
  tim_exit("mp2-f12/a pair energies");
  if (stdapprox_ == LinearR12::StdApprox_Ap ||
      stdapprox_ == LinearR12::StdApprox_B) {
    tim_enter("mp2-f12/a' pair energies");
    if (r12ap_energy_.null())
      r12ap_energy_ = new MP2R12Energy(r12eval_,LinearR12::StdApprox_Ap,debug_);
    r12ap_energy_->print_pair_energies(spinadapted_);

#if MP2R12ENERGY_CAN_COMPUTE_PAIRFUNCTION
    if (twopdm_grid_aa_.nonnull())
      r12ap_energy_->compute_pair_function_aa(0,twopdm_grid_aa_);
    if (twopdm_grid_ab_.nonnull())
      r12ap_energy_->compute_pair_function_ab(0,twopdm_grid_ab_);
#endif

    etotal = r12ap_energy_->energy();
    tim_exit("mp2-f12/a' pair energies");
  }
  if (stdapprox_ == LinearR12::StdApprox_B) {
    tim_enter("mp2-f12/b pair energies");
    if (r12b_energy_.null())
      r12b_energy_ = new MP2R12Energy(r12eval_,LinearR12::StdApprox_B,debug_);
    r12b_energy_->print_pair_energies(spinadapted_);
    etotal = r12b_energy_->energy();
    tim_exit("mp2-f12/b pair energies");
  }
  
  tim_exit("mp2-f12 energy");

  etotal += ref_energy();
  set_energy(etotal);
  set_actual_value_accuracy(reference_->actual_value_accuracy()
                            *ref_to_mp2_acc);
  
  return;
}


////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
