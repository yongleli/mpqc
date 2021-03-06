//
// Created by Chong Peng on 9/9/16.
//

#include <clocale>
#include <iostream>

#include <madness/world/world.h>
#include <mpqc/chemistry/qc/lcao/wfn/wfn.h>

#include <mpqc/chemistry/molecule/molecule.h>
#include <mpqc/chemistry/molecule/linkage.h>
#include <mpqc/util/external/madworld/parallel_file.h>
#include <mpqc/chemistry/qc/lcao/basis/basis_registry.h>
#include <mpqc/util/keyval/keyval.h>

using namespace mpqc;
using mpqc::lcao::gaussian::Basis;

double bytes_to_gb(std::size_t num) { return double(num) / (1e9); }

double bytes_to_mb(std::size_t num) { return double(num) / (1e6); }

void ao_basis_analysis(const Basis& basis, int o, int nfr) {
  auto range = basis.create_trange1();
  std::size_t n = range.elements_range().second;
  std::size_t v = n - o;
  std::size_t occ = o - nfr;
  auto min_max = detail::minmax_blocksize(range);
  std::size_t b_min = min_max.first;
  std::size_t b_max = min_max.second;

  std::cout << "OBS" << std::endl;
  std::cout << "Min Block Size: "
            << bytes_to_mb(b_min * b_min * b_min * b_min * 8) << " MB"
            << std::endl;
  std::cout << "Max Block Size: "
            << bytes_to_mb(b_max * b_max * b_max * b_max * 8) << " MB"
            << std::endl;

  std::cout << "NNNN: " << bytes_to_gb(n * n * n * n * 8) << " GB" << std::endl;
  std::cout << std::endl;
  std::cout << "OOOO: " << bytes_to_gb(occ * occ * occ * occ * 8) << " GB"
            << std::endl;
  std::cout << "OOOV: " << bytes_to_gb(occ * occ * occ * v * 8) << " GB"
            << std::endl;
  std::cout << "OOVV: " << bytes_to_gb(occ * occ * v * v * 8) << " GB"
            << std::endl;
  std::cout << "OOPP: " << bytes_to_gb(occ * occ * n * n * 8) << " GB"
            << std::endl;
  std::cout << "OOVP: " << bytes_to_gb(occ * occ * n * v * 8) << " GB"
            << std::endl;
  std::cout << "OVVV: " << bytes_to_gb(occ * v * v * v * 8) << " GB"
            << std::endl;
  std::cout << "VVVV: " << bytes_to_gb(v * v * v * v * 8) << " GB" << std::endl;
  std::cout << "VVPP: " << bytes_to_gb(v * v * n * n * 8) << " GB" << std::endl;
  std::cout << std::endl;
}

void df_basis_analysis(const Basis& basis, const Basis& dfbs, std::size_t o,
                       std::size_t nfr) {
  auto range = basis.create_trange1();
  std::size_t n = range.elements_range().second;
  std::size_t v = n - o;
  std::size_t occ = o - nfr;
  auto min_max = detail::minmax_blocksize(range);
  std::size_t b_min = min_max.first;
  std::size_t b_max = min_max.second;

  auto df_range = dfbs.create_trange1();
  std::size_t N = df_range.elements_range().second;
  auto min_max2 = detail::minmax_blocksize(df_range);
  std::size_t b_min2 = min_max2.first;
  std::size_t b_max2 = min_max2.second;

  std::cout << "DFBS" << std::endl;

  std::cout << "Min Block Size: " << bytes_to_mb(b_min2 * b_min * b_min * 8)
            << " MB" << std::endl;
  std::cout << "Max Block Size: " << bytes_to_mb(b_max2 * b_max * b_max * 8)
            << " MB" << std::endl;

  std::cout << "ΛNN: " << bytes_to_gb(N * n * n * 8) << " GB" << std::endl;
  std::cout << std::endl;
  std::cout << "ΛOO: " << bytes_to_gb(N * occ * occ * 8) << " GB" << std::endl;
  std::cout << "ΛOV: " << bytes_to_gb(N * occ * v * 8) << " GB" << std::endl;
  std::cout << "ΛVV: " << bytes_to_gb(N * v * v * 8) << " GB" << std::endl;
  std::cout << std::endl;
}

void cabs_basis_analysis(const Basis& basis, const Basis& dfbs,
                         const Basis& cabs, std::size_t o, std::size_t nfr) {
  auto range = basis.create_trange1();
  std::size_t n = range.elements_range().second;
  std::size_t v = n - o;
  std::size_t occ = o - nfr;

  auto df_range = dfbs.create_trange1();
  std::size_t N = df_range.elements_range().second;
  auto min_max2 = detail::minmax_blocksize(df_range);
  std::size_t b_min2 = min_max2.first;
  std::size_t b_max2 = min_max2.second;

  auto cabs_range = cabs.create_trange1();
  std::size_t A = cabs_range.elements_range().second + n;
  auto min_max3 = detail::minmax_blocksize(cabs_range);
  std::size_t b_min3 = min_max3.first;
  std::size_t b_max3 = min_max3.second;

  std::size_t V = N - occ;

  std::cout << "CABS" << std::endl;

  std::cout << "Min Block Size: " << bytes_to_mb(b_min2 * b_min3 * b_min3 * 8)
            << " MB" << std::endl;
  std::cout << "Max Block Size: " << bytes_to_mb(b_max2 * b_max3 * b_max3 * 8)
            << " MB" << std::endl;

  std::cout << "ΛAN: " << bytes_to_gb(N * A * n * 8) << " GB" << std::endl;
  std::cout << "ΛAA: " << bytes_to_gb(N * A * A * 8) << " GB" << std::endl;
  std::cout << std::endl;

  std::cout << "ΛOV': " << bytes_to_gb(N * occ * V * 8) << " GB" << std::endl;
  std::cout << "OOV'V': " << bytes_to_gb(occ * occ * V * V * 8) << " GB"
            << std::endl;
  std::cout << "OOOV': " << bytes_to_gb(occ * occ * occ * V * 8) << " GB"
            << std::endl;
  std::cout << "OOVV': " << bytes_to_gb(occ * occ * v * V * 8) << " GB"
            << std::endl;
  std::cout << "OOPP': " << bytes_to_gb(occ * occ * n * A * 8) << " GB"
            << std::endl;
  std::cout << "OOP'P': " << bytes_to_gb(occ * occ * A * A * 8) << " GB"
            << std::endl;
}

int try_main(int argc, char* argv[], madness::World& world) {
  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " <input_file.json>" << std::endl;
    throw std::invalid_argument("no input file given");
  }

  std::stringstream ss;
  utility::parallel_read_file(world, argv[1], ss);

  KeyVal kv;
  kv.read_json(ss);
  kv.assign("world", &world);

  auto wfn_world = lcao::WavefunctionWorld(kv.keyval("property:wfn:wfn_world"));

  auto mol = wfn_world.atoms();

  bool frozen_core = kv.value<bool>("property:wfn:frozen_core", true);

  std::size_t occ = mol->total_atomic_number() / 2;
  std::size_t nfr = frozen_core ? mol->core_electrons() / 2 : 0;

  std::cout << "Num Occ: " << occ << std::endl;
  std::cout << "Num Frozen: " << nfr << std::endl;

  auto basis_registry = wfn_world.basis_registry();

  const auto& obs = basis_registry->retrieve(OrbitalIndex(L"λ"));
  ao_basis_analysis(*obs, occ, nfr);

  const auto dfbs_index = OrbitalIndex(L"Κ");
  if(basis_registry->have(dfbs_index)){
    const auto& dfbs = basis_registry->retrieve(dfbs_index);
    df_basis_analysis(*obs, *dfbs, occ, nfr);
  }

  const auto& cabs_index = OrbitalIndex(L"α");
  if(basis_registry->have(cabs_index)){
    const auto& dfbs = basis_registry->retrieve(dfbs_index);
    const auto& cabs = basis_registry->retrieve(cabs_index);
    cabs_basis_analysis(*obs, *dfbs, *cabs, occ, nfr);
  }
  return 0;
}

int main(int argc, char* argv[]) {
  int rc = 0;

  auto& world = madness::initialize(argc, argv);
  mpqc::utility::print_par(world, "MADNESS process total size: ", world.size(),
                           "\n");

  std::setlocale(LC_ALL, "en_US.UTF-8");
  std::cout << std::setprecision(15);
  std::wcout.sync_with_stdio(false);
  std::wcerr.sync_with_stdio(false);
  std::wcout.imbue(std::locale("en_US.UTF-8"));
  std::wcerr.imbue(std::locale("en_US.UTF-8"));
  std::wcout.sync_with_stdio(true);
  std::wcerr.sync_with_stdio(true);

  try {
    try_main(argc, argv, world);

  } catch (TiledArray::Exception& e) {
    std::cerr << "!! TiledArray exception: " << e.what() << "\n";
    rc = 1;
  } catch (madness::MadnessException& e) {
    std::cerr << "!! MADNESS exception: " << e.what() << "\n";
    rc = 1;
  } catch (SafeMPI::Exception& e) {
    std::cerr << "!! SafeMPI exception: " << e.what() << "\n";
    rc = 1;
  } catch (std::exception& e) {
    std::cerr << "!! std exception: " << e.what() << "\n";
    rc = 1;
  } catch (...) {
    std::cerr << "!! exception: unknown exception\n";
    rc = 1;
  }

  madness::finalize();
  return rc;
}
