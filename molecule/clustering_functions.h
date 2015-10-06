#pragma once
#ifndef MPQC_CLUSTERING_FUNCTIONS_H
#define MPQC_CLUSTERING_FUNCTIONS_H

#include "molecule_fwd.h"
#include "../clustering/clustering_fwd.h"

#include <vector>

namespace mpqc {
namespace molecule {

Molecule attach_hydrogens_and_kmeans(
      std::vector<AtomBasedClusterable> const &, int64_t);

} // namespace molecule
} // namespace tcc

#endif // MPQC_CLUSTERING_FUNCTIONS_H
