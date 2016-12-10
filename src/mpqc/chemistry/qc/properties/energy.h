/*
 * energy.h
 *
 *  Created on: Aug 18, 2016
 *      Author: Drew Lewis
 */
#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_PROPERTIES_ENERGY_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_PROPERTIES_ENERGY_H_

#include "mpqc/util/keyval/keyval.h"
#include "mpqc/chemistry/qc/properties/propertybase.h"

#include <boost/optional.hpp>

namespace mpqc {
namespace lcao {

class Energy : public PropertyBase {
 private:
  boost::optional<double> result_;

 public:
  Energy(KeyVal const &kv);

  void apply(Wavefunction *) override;

  boost::optional<double> result() { return result_; }
};

}  // namespace lcao
}  // namespace mpqc

#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_PROPERTIES_ENERGY_H_
