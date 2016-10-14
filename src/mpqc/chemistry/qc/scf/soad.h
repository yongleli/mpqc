#pragma once
#ifndef TCC_INTEGRALS_SCF_SOAD_H
#define TCC_INTEGRALS_SCF_SOAD_H

#include <libint2/chemistry/sto3g_atomic_density.h>

#include <mpqc/chemistry/molecule/molecule.h>
#include <mpqc/chemistry/qc/basis/basis.h>
#include <mpqc/chemistry/qc/basis/basis_set.h>

#include "../../../../../common/typedefs.h"
#include "../../../../../include/tiledarray.h"
#include "../../../../../utility/array_info.h"
#include "../../../../../utility/make_array.h"

#include "../../../../../common/namespaces.h"

#include <mpqc/chemistry/qc/integrals/direct_task_integrals.h>
#include <mpqc/chemistry/qc/integrals/task_integrals.h>
#include <mpqc/chemistry/qc/integrals/task_integrals_common.h>

#include "../../../../../ta_routines/array_to_eigen.h"

#include <memory>
#include <vector>

namespace mpqc {
namespace scf {

MatrixD soad_density_eig_matrix(molecule::Molecule const &mol) {
  auto nao = 0;
  for (const auto &atom : mol.atoms()) {
    const auto Z = atom.charge();
    nao += libint2::sto3g_num_ao(Z);
  }

  MatrixD D(nao, nao);
  D.setZero();

  size_t ao = 0;
  for (const auto &atom : mol.atoms()) {
    const auto Z = atom.charge();
    const auto &occvec = libint2::sto3g_ao_occupation_vector(Z);
    for (const auto &occ : occvec) {
      D(ao, ao) = occ;
      ++ao;
    }
  }

  return D * 0.5;  // we use densities normalized to # of electrons/2
}

template <typename Engs, typename Array, typename Tile>
void soad_task(Engs eng_pool, int64_t ord, ShellVec const *obs_row,
               ShellVec const *obs_col, ShellVec const *min_bs,
               const MatrixD *D, Array *F,
               std::function<Tile(TA::TensorD &&)> op) {
  auto range = F->trange().make_tile_range(ord);
  const auto lb = range.lobound();
  TA::TensorD tile(range, 0.0);

  auto &d = *D;

  auto &eng = eng_pool->local();
  eng.set_precision(1e-12);

  using Rng = std::pair<int64_t, int64_t>;

  auto J = [&](double const *buff, Rng sh0_rng, Rng sh1_rng, Rng sh_min_rng) {
    const auto row_start = sh0_rng.first;
    const auto col_start = sh1_rng.first;
    const auto min_start = sh_min_rng.first;

    const auto row_end = sh0_rng.second;
    const auto col_end = sh1_rng.second;
    const auto min_end = sh_min_rng.second;

    const auto row_size = row_end - row_start;
    const auto col_size = col_end - col_start;
    const auto min_size = min_end - min_start;

    for (auto p = 0, j_ord = 0; p < row_size; ++p) {
      const auto fp = p + row_start + lb[0];

      for (auto q = 0; q < col_size; ++q) {
        const auto fq = q + col_start + lb[1];

        auto val = 0.0;
        for (auto r = 0; r < min_size; ++r) {
          const auto dr = r + min_start;

          for (auto s = 0; s < min_size; ++s, ++j_ord) {
            const auto ds = s + min_start;

            val += buff[j_ord] * d(dr, ds);
          }
        }

        tile(fp, fq) += 2 * val;
      }
    }
  };

  auto K = [&](double const *buff, Rng sh0_rng, Rng sh1_rng, Rng sh_min_rng) {
    const auto row_start = sh0_rng.first;
    const auto col_start = sh1_rng.first;
    const auto min_start = sh_min_rng.first;

    const auto row_end = sh0_rng.second;
    const auto col_end = sh1_rng.second;
    const auto min_end = sh_min_rng.second;

    const auto row_size = row_end - row_start;
    const auto col_size = col_end - col_start;
    const auto min_size = min_end - min_start;

    for (auto p = 0, k_ord = 0; p < row_size; ++p) {
      const auto fp = p + row_start + lb[0];

      for (auto r = 0; r < min_size; ++r) {
        const auto dr = r + min_start;

        for (auto q = 0; q < col_size; ++q) {
          const auto fq = q + col_start + lb[1];

          for (auto s = 0; s < min_size; ++s, ++k_ord) {
            const auto ds = s + min_start;

            tile(fp, fq) -= buff[k_ord] * d(dr, ds);
          }
        }
      }
    }

  };

  // Loop over shells
  auto sh0_start = 0;
  for (auto const &sh0 : *obs_row) {
    const auto sh0_size = sh0.size();
    auto sh0_rng = std::make_pair(sh0_start, sh0_start + sh0_size);
    sh0_start += sh0_size;

    auto sh1_start = 0;
    for (auto const &sh1 : *obs_col) {
      const auto sh1_size = sh1.size();
      auto sh1_rng = std::make_pair(sh1_start, sh1_start + sh1_size);
      sh1_start += sh1_size;

      auto min_start = 0;
      for (auto const &sh_min : *min_bs) {
        const auto min_size = sh_min.size();
        auto min_rng = std::make_pair(min_start, min_start + min_size);
        min_start += min_size;

        const auto &J_bufs = eng.compute(sh0, sh1, sh_min, sh_min);
        TA_USER_ASSERT(J_bufs.size() == 1,
                       "unexpected result from Engine::compute()");
        if (J_bufs[0] != nullptr) {
          J(J_bufs[0], sh0_rng, sh1_rng, min_rng);
        }

        const auto &K_bufs = eng.compute(sh0, sh_min, sh1, sh_min);
        TA_USER_ASSERT(K_bufs.size() == 1,
                       "unexpected result from Engine::compute()");
        if (K_bufs[0] != nullptr) {
          K(K_bufs[0], sh0_rng, sh1_rng, min_rng);
        }
      }
    }
  }

  F->set(ord, op(std::move(tile)));
  eng.set_precision(integrals::detail::integral_engine_precision);
}

template <typename ShrPool, typename Array, typename Tile = TA::TensorD>
Array fock_from_soad(madness::World &world,
                     molecule::Molecule const &clustered_mol,
                     basis::Basis const &obs, ShrPool engs, Array const &H,
                     std::function<Tile(TA::TensorD &&)> op =
                         mpqc::ta_routines::TensorDPassThrough()) {
  // Soad Density
  auto D = soad_density_eig_matrix(clustered_mol);

  // Get minimal basis
  const auto min_bs_shells =
      parallel_construct_basis(world, basis::BasisSet("sto-3g"), clustered_mol)
          .flattened_shells();
  // Make F scaffolding
  auto const &trange = H.trange();
  auto const &shape_range = H.shape().data().range();

  const auto max_norm = std::numeric_limits<float>::max();
  auto shape_norms = TA::Tensor<float>(shape_range, max_norm);
  TA::SparseShape<float> F_shape(shape_norms, trange);

  Array F(world, trange, F_shape);

  // Loop over lower diagonal tiles
  const auto F_extent = F.trange().tiles_range().extent();
  for (auto i = 0; i < F_extent[0]; ++i) {
    const auto i_ord = i * F_extent[1];

    for (auto j = 0; j < F_extent[1]; ++j) {
      const auto ord = i_ord + j;

      if (!F.is_zero(ord) && F.is_local(ord)) {
        auto const &obs_row = obs.cluster_shells()[i];
        auto const &obs_col = obs.cluster_shells()[j];

        world.taskq.add(soad_task<ShrPool, Array, Tile>, engs, ord, &obs_row,
                        &obs_col, &min_bs_shells, &D, &F, op);
      }
    }
  }
  world.gop.fence();

  F("i,j") += H("i,j");
  F.truncate();
  return F;
}

}  // namespace scf
}  // namespace mpqc

#endif  // TCC_INTEGRALS_SCF_SOAD_H
