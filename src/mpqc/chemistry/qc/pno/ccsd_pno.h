/*
 * ccsd_pno.h
 *
 *  Created on: Jan 4, 2017
 *      Author: jinmei
 */

#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_PNO_CCSD_PNO_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_PNO_CCSD_PNO_H_

#include <iostream>
#include <sstream>
#include <tiledarray.h>

#include "mpqc/chemistry/qc/cc/ccsd.h"
#include "mpqc/chemistry/qc/mbpt/denom.h"
#include "mpqc/chemistry/qc/scf/mo_build.h"
#include "mpqc/chemistry/qc/wfn/lcao_wfn.h"
#include "mpqc/math/tensor/clr/decomposed_tensor_algebra.h"

namespace mpqc {
namespace lcao {

  /**
   *  \brief CCSD_PNO class
   *
   *  keyword to call this class CCSD-PNO
   */

  template <typename Tile, typename Policy>
  class CCSD_PNO : public CCSD<Tile, Policy> {

  private:
   double tcut_;
   TA::DistArray<Tile, Policy>  t2_mp2_;

   // compute MP2 T2 amplitudes
   void compute_mp2_t2();
   // reblock MP2 T2
//   void reblock();
   // compute the occ and vir matrices for reblocking T2
   void compute_M_reblock(TA::DistArray<Tile, Policy> &occ_convert,
                          TA::DistArray<Tile, Policy> &vir_convert);
   // svd decompostion of T2
   void pno_decom();

   // compute CCSD T2 amplitudes from PNO constructed T2
   // based on Chong's compute_ccsd_df(TArray &t1, TArray &t2) function
   double compute_ccsdpno_df(TA::DistArray<Tile, Policy> &t1,
                             TA::DistArray<Tile, Policy> &t2);

  public:
   CCSD_PNO() = default;
   CCSD_PNO(const KeyVal &kv);

   ~CCSD_PNO() = default;
   void compute(PropertyBase* pb) override;
   // compute function
   double value() override;

  };

//#if TA_DEFAULT_POLICY == 0
//extern template class CCSD_PNO<TA::TensorD, TA::DensePolicy>;
//#elif TA_DEFAULT_POLICY == 1
//extern template class CCSD_PNO<TA::TensorD, TA::SparsePolicy>;
//#endif

}  // namespace lcao
}  // namespace mpqc

#include "ccsd_pno_impl.h"

#endif // MPQC4_SRC_MPQC_CHEMISTRY_QC_PNO_CCSD_PNO_H_
