#include "schwarz_screen.h"

#include "mpqc/math/groups/petite_list.h"
#include "mpqc/util/misc/exenv.h"


namespace mpqc {
namespace lcao {
namespace gaussian {

int64_t Qmatrix::f2s(Qmatrix::f2s_map const &map, int64_t ind) const {
  auto it = map.find(ind);
  // If this hits the issue was likely an index that was not the
  // first function in the shell.
  assert(it != map.end());
  return it->second;
}

double Qmatrix::operator()() const { return max_elem_in_Q_; }

double Qmatrix::operator()(int64_t a) const {
  return max_elem_in_row_[f2s(f2s_maps_[0], a)];
}

double Qmatrix::max_in_row(int64_t a) const { return max_elem_in_row_[a]; }

double Qmatrix::operator()(int64_t a, int64_t b) const {
  return Q_(f2s(f2s_maps_[0], a), f2s(f2s_maps_[1], b));
}

bool Qmatrix::is_aux_Q() const { return is_aux_; }

SchwarzScreen::SchwarzScreen(std::shared_ptr<Qmatrix> Qbra,
                             std::shared_ptr<Qmatrix> Qket, double thresh)
    : Screener(),
      Qbra_(std::move(Qbra)),
      Qket_(std::move(Qket)),
      thresh_(thresh),
      thresh2_(thresh_ * thresh_) {}

double SchwarzScreen::skip_threshold() const { return thresh_; }

bool SchwarzScreen::skip(int64_t a) { return skip_(a); }
bool SchwarzScreen::skip(int64_t a) const { return skip_(a); }

bool SchwarzScreen::skip(int64_t a, int64_t b) { return skip_(a, b); }
bool SchwarzScreen::skip(int64_t a, int64_t b) const { return skip_(a, b); }

bool SchwarzScreen::skip(int64_t a, int64_t b, int64_t c) {
  return skip_(a, b, c);
}
bool SchwarzScreen::skip(int64_t a, int64_t b, int64_t c) const {
  return skip_(a, b, c);
}

bool SchwarzScreen::skip(int64_t a, int64_t b, int64_t c, int64_t d) {
  return skip_(a, b, c, d);
}
bool SchwarzScreen::skip(int64_t a, int64_t b, int64_t c, int64_t d) const {
  return skip_(a, b, c, d);
}

boost::optional<double> SchwarzScreen::estimate(int64_t a) const {
  return Qbra()(a) * Qket()();
}

boost::optional<double> SchwarzScreen::estimate(int64_t a, int64_t b) const {
  if (Qbra_->is_aux_Q()) {
    return Qbra()(a) * Qket()(b);
  } else {
    return Qbra()(a, b) * Qket()();
  }
}

boost::optional<double> SchwarzScreen::estimate(int64_t a, int64_t b,
                                                int64_t c) const {
  if (Qbra_->is_aux_Q()) {
    return Qbra()(a) * Qket()(b, c);
  } else {
    return Qbra()(a, b) * Qket()(c);
  }
}

boost::optional<double> SchwarzScreen::estimate(int64_t a, int64_t b, int64_t c,
                                                int64_t d) const {
  assert(!Qbra_->is_aux_Q());
  return Qbra()(a, b) * Qket()(c, d);
}

TA::Tensor<float> SchwarzScreen::norm_estimate(
    madness::World &world, std::vector<gaussian::Basis> const &bs_array,
    TA::Pmap const &pmap, const math::PetiteList &plist, bool replicate) const {
  const auto ndims = bs_array.size();
  auto trange = gaussian::detail::create_trange(bs_array);
  auto const &tile_range = trange.tiles_range();
  auto norms = TA::Tensor<float>(trange.tiles_range(), 0.0);

  if (ndims == 3) {
    auto const &Ta = Qbra_->Qtile();
    auto const &Tbc = Qket_->Qtile();
    for (auto a = 0ul; a < Ta.size(); ++a) {
      const float a_val = Ta(a);
      for (auto b = 0ul; b < Tbc.cols(); ++b) {
        for (auto c = 0ul; c < Tbc.rows(); ++c) {
          const auto ord =
              tile_range.ordinal(std::array<decltype(a), 3>{a, b, c});
          if (pmap.is_local(ord)){ // && plist.is_canonical(a, b, c)) {
            // const float multiplicity = plist.multiplicity(a, b, c);
            const float multiplicity = 1.0;
            norms[ord] = multiplicity * std::sqrt(a_val * Tbc(b, c));
          }
        }
      }
    }
  } else if (ndims == 4) {
    auto const &Tab = Qbra_->Qtile();
    auto const &Tcd = Qket_->Qtile();
    for (auto a = 0ul; a < Tab.rows(); ++a) {
      for (auto b = 0ul; b < Tab.cols(); ++b) {
        const float ab = Tab(a, b);
        for (auto c = 0ul; c < Tcd.rows(); ++c) {
          for (auto d = 0ul; d < Tcd.cols(); ++d) {
            const auto ord =
                tile_range.ordinal(std::array<decltype(a), 4>{a, b, c, d});
            if (pmap.is_local(ord)){ // && plist.is_canonical(a, b, c, d)) {
              // const float multiplicity = plist.multiplicity(a, b, c, d);
              const float multiplicity = 1.0;
              norms[ord] = multiplicity * std::sqrt(ab * Tcd(c, d));
            }
          }
        }
      }
    }
  } else {
    norms = Screener::norm_estimate(world, bs_array, pmap, plist);
  }
  world.gop.fence();

  if (replicate) {  // construct the sum
    world.gop.sum(norms.data(), norms.size());
  }
  world.gop.fence();

  return norms;
}

}  // namespace  gaussian
}  // namespace  lcao
}  // namespace  mpqc
