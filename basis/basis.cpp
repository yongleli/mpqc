#include "../molecule/cluster.h"

#include "basis.h"
#include "basis_set.h"
#include "shell_vec_functions.h"

#include "../include/libint.h"
#include "../include/tiledarray.h"

#include <vector>

namespace mpqc {
namespace basis {

Basis::Basis() = default;
Basis::~Basis() = default;
Basis::Basis(Basis const &) = default;
Basis::Basis(Basis &&) = default;

Basis &Basis::operator=(Basis const &) = default;
Basis &Basis::operator=(Basis &&) = default;

Basis::Basis(std::vector<ShellVec> shells) : shells_(std::move(shells)) {}

TiledArray::TiledRange1 Basis::create_trange1() const {
    auto blocking = std::vector<int64_t>{0};
    for (auto const &shell_vec : shells_) {
        auto next = blocking.back() + basis::nfunctions(shell_vec);
        blocking.emplace_back(next);
    }

    return TiledArray::TiledRange1(blocking.begin(), blocking.end());
}

int64_t Basis::max_nprim() const {
    int64_t max = 0;
    for (auto const &shell_vec : shells_) {
        const auto current = basis::max_nprim(shell_vec);
        max = std::max(current, max);
    }
    return max;
}

int64_t Basis::max_am() const {
    int64_t max = 0;
    for (auto const &shell_vec : shells_) {
        const auto current = basis::max_am(shell_vec);
        max = std::max(current, max);
    }
    return max;
}


std::vector<ShellVec> const &Basis::cluster_shells() const {
    return shells_;
}

std::ostream &operator<<(std::ostream &os, Basis const &b) {
    unsigned int n = 0;
    for (auto const &shell_vec : b.cluster_shells()) {
        os << "Cluster " << n << "\n";
        ++n;

        for (auto const &shell : shell_vec) {
            os << shell << "\n";
        }
    }

    return os;
}

} // namespace basis
} // namespace basis
