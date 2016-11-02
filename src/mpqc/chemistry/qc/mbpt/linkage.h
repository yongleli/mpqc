//
// Created by Chong Peng on 10/6/16.
//

#ifndef SRC_MPQC_CHEMISTRY_QC_MBPT_LINKAGE_H_
#define SRC_MPQC_CHEMISTRY_QC_MBPT_LINKAGE_H_

#include "mpqc/chemistry/qc/scf/linkage.h"
#include "mpqc/util/keyval/forcelink.h"

namespace mpqc {
namespace mbpt {
class RMP2;
class RIRMP2;
mpqc::detail::ForceLink<RMP2> fl1;
mpqc::detail::ForceLink<RIRMP2> fl2;
}
}

#endif  // SRC_MPQC_CHEMISTRY_QC_MBPT_LINKAGE_H_
