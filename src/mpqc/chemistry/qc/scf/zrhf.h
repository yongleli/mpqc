#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_SCF_ZRHF_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_SCF_ZRHF_H_

#include "mpqc/chemistry/qc/integrals/periodic_ao_factory.h"
#include "mpqc/chemistry/qc/integrals/periodic_lcao_factory.h"
#include "mpqc/chemistry/qc/wfn/trange1_engine.h"

#include <memory>

#include <tiledarray.h>

#include "mpqc/chemistry/qc/wfn/ao_wfn.h"
#include "mpqc/util/external/c++/memory"

namespace mpqc {
namespace lcao {

using MatrixzVec = std::vector<Matrixz>;
using VectorzVec = std::vector<Vectorz>;

/*!
 * \brief This inserts crystal orbitals to registry for gamma-point methods
 */
template <typename Tile, typename Policy>
void mo_insert_gamma_point(PeriodicLCAOFactory<Tile, Policy>& plcao_factory,
                           Matrixz& C_gamma_point, Molecule& unitcell,
                           size_t occ_block, size_t vir_block) {
  auto& orbital_registry = plcao_factory.orbital_space();
  auto& world = plcao_factory.world();

  auto all = C_gamma_point.cols();
  auto occ = unitcell.occupation() / 2;
  auto vir = all - occ;
  std::size_t n_frozen_core = 0;  // TODO: should be determined by user

  Matrixz C_occ = C_gamma_point.leftCols(occ);
  Matrixz C_corr_occ =
      C_gamma_point.block(0, n_frozen_core, all, occ - n_frozen_core);
  Matrixz C_vir = C_gamma_point.rightCols(vir);

  ExEnv::out0() << "OccBlockSize: " << occ_block << std::endl;
  ExEnv::out0() << "VirBlockSize: " << vir_block << std::endl;

  auto tre = std::make_shared<TRange1Engine>(occ, all, occ_block, vir_block, 0);

  // get all trange1s
  auto tr_obs = tre->get_all_tr1();  // TODO: this is not right
  auto tr_occ = tre->get_occ_tr1();
  auto tr_corr_occ = tre->get_active_occ_tr1();
  auto tr_vir = tre->get_vir_tr1();
  auto tr_all = tre->get_all_tr1();

  mpqc::detail::parallel_print_range_info(world, tr_occ, "Occ");
  mpqc::detail::parallel_print_range_info(world, tr_corr_occ, "CorrOcc");
  mpqc::detail::parallel_print_range_info(world, tr_vir, "Vir");
  mpqc::detail::parallel_print_range_info(world, tr_all, "All");

  // convert Eigen matrices to TA
  auto C_occ_ta =
      array_ops::eigen_to_array<Tile, Policy>(world, C_occ, tr_obs, tr_occ);
  auto C_corr_occ_ta = array_ops::eigen_to_array<Tile, Policy>(
      world, C_corr_occ, tr_obs, tr_corr_occ);
  auto C_vir_ta =
      array_ops::eigen_to_array<Tile, Policy>(world, C_vir, tr_obs, tr_vir);
  auto C_all_ta = array_ops::eigen_to_array<Tile, Policy>(world, C_gamma_point,
                                                          tr_obs, tr_all);

  // insert to registry
  using OrbitalSpaceTArray = OrbitalSpace<TA::DistArray<Tile, Policy>>;
  auto occ_space =
      OrbitalSpaceTArray(OrbitalIndex(L"m"), OrbitalIndex(L"κ"), C_occ_ta);
  orbital_registry.add(occ_space);

  auto corr_occ_space =
      OrbitalSpaceTArray(OrbitalIndex(L"i"), OrbitalIndex(L"κ"), C_corr_occ_ta);
  orbital_registry.add(corr_occ_space);

  auto vir_space =
      OrbitalSpaceTArray(OrbitalIndex(L"a"), OrbitalIndex(L"κ"), C_vir_ta);
  orbital_registry.add(vir_space);

  auto all_space =
      OrbitalSpaceTArray(OrbitalIndex(L"p"), OrbitalIndex(L"κ"), C_all_ta);
  orbital_registry.add(all_space);
}

/**
 * complex-valued Restricted Hartree-Fock class
 */

class zRHF : public PeriodicAOWavefunction<TA::TensorZ, TA::SparsePolicy> {
 public:
  using Tile = TA::TensorZ;
  using TArray =
      PeriodicAOWavefunction<TA::TensorZ, TA::SparsePolicy>::ArrayType;
  using PeriodicAOIntegral = PeriodicAOWavefunction::AOIntegral;

  zRHF() = default;

  /**
   * KeyVal constructor for zRHF
   *
   * keywords: takes all keywords from PeriodicAOWavefunction
   *
   * | KeyWord | Type | Default| Description |
   * |---------|------|--------|-------------|
   * | converge | double | 1.0e-07 | converge limit |
   * | max_iter | int | 30 | maximum number of iteration |
   * | soad_guess | bool | true | if use SOAD guess for initial Fock build |
   * | print_detail | bool | false | if print extra computation&time info |
   * | max_condition_num | double | 1.0e8 | maximum condition number for overlap
   * matrix |
   *
   */
  zRHF(const KeyVal& kv);

  ~zRHF() = default;

  void compute(lcao::PropertyBase* pb) override;
  void obsolete() override;

  /*!
   * \brief This performs a Hartree-Fock computation
   * \return the Hartree-Fock energy
   */
  double value() override;

  /// return crystal orbital coefficients
  MatrixzVec co_coeff() override { return C_; }

  /// return crystal orbital energies
  VectorzVec co_energy() override { return eps_; }

  /// return # of k points
  Vector3i nk() override { return nk_; }

 private:
  /*!
   * \brief This performs SCF procedure for zRHF
   * \return true if SCF converges, false if not
   */
  bool solve();

  /*!
   * \brief This diagonalizes Fock matrix in reciprocal space and
   * computes density: D_ = Int_k( Exp(I k.R) C(occ).C(occ)t )
   */
  TArray compute_density();

  /*!
   * \brief This transforms an integral matrix from real to reciprocal space
   * \param matrix the real-space integral matrix
   * \return the reciprocal-space integral matrix
   */
  TArray transform_real2recip(TArray& matrix);

  TArray T_;
  TArray V_;
  TArray S_;
  TArray Sk_;
  TArray H_;
  TArray Hk_;
  TArray J_;
  TArray K_;
  TArray F_;
  TArray Fk_;
  TArray D_;

  MatrixzVec C_;
  VectorzVec eps_;
  MatrixzVec X_;

  double repulsion_;
  int64_t docc_;

  const KeyVal kv_;
  double converge_;
  int64_t maxiter_;
  bool print_detail_;
  double max_condition_num_;

  Vector3i R_max_;   ///> range of expansion of Bloch Gaussians in AO Gaussians
  Vector3i RJ_max_;  ///> range of Coulomb operation
  Vector3i RD_max_;  ///> range of density representation
  Vector3i nk_ = {1, 1, 1};  ///> # of k points in each direction
  Vector3d dcell_;           ///> direct unit cell params (in a.u.)
  int64_t
      R_size_;  ///> cardinal # of lattices included in Bloch Gaussian expansion
  int64_t RJ_size_;  ///> cardinal # of lattices included in Coulomb operation
  int64_t
      RD_size_;  ///> cardinal # of lattices included in density representation
  int64_t k_size_;  ///> cardinal # of k points

  double init_duration_ = 0.0;
  double j_duration_ = 0.0;
  double k_duration_ = 0.0;
  double trans_duration_ = 0.0;
  double d_duration_ = 0.0;
  double scf_duration_ = 0.0;

  /*!
   * \brief This initialize zRHF by assigning values to private members
   * and computing initial guess for the density
   *
   * \param kv KeyVal object
   */
  void init(const KeyVal& kv);
};

}  // namespace  lcao
}  // namespace  mpqc
#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_SCF_ZRHF_H_
