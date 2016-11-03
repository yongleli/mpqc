#ifndef MPQC_SCF_BUILDER_H
#define MPQC_SCF_BUILDER_H

#include <string>

#include <tiledarray.h>

#include "mpqc/chemistry/qc/expression/formula_registry.h"

namespace mpqc {
namespace scf {

class FockBuilder {
  public:
    using array_type = TA::TSpArrayD;
    virtual ~FockBuilder() = default;

    virtual array_type operator()(array_type const&, array_type const&) = 0;

    virtual void print_iter(std::string const &) = 0;

    virtual void register_fock(const TA::TSpArrayD &, FormulaRegistry<TA::TSpArrayD>& ) = 0;

};

} // namespace scf
} // namespace mpqc


#endif // MPQC_SCF_BUILDER_H