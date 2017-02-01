#include "mpqc/chemistry/molecule/atom.h"

#include <iostream>
#include <map>

#include "mpqc/chemistry/molecule/atom_masses.h"
#include "mpqc/chemistry/units/units.h"
#include <libint2/chemistry/elements.h>

namespace mpqc {

namespace detail {

std::string Z_to_element(int64_t Z) {
  using libint2::chemistry::element_info;
  for (const auto &e : element_info) {
    if (e.Z == Z) return e.symbol;
  }
  abort();
}

int64_t element_to_Z(const std::string& symbol) {
  using libint2::chemistry::element_info;
  for (const auto &e : element_info) {
    if (e.symbol == symbol) return e.Z;
  }
  abort();
}

}  // namespace detail

std::string Atom::xyz_string(bool convert_to_angstroms) const {
  std::string result = detail::Z_to_element(atomic_number_);
  result += ' ';

  auto unit_factory = UnitFactory::get_default();
  auto bohr_to_ang = unit_factory->make_unit("angstrom").from_atomic_units();

  Vector3d center = center_;
  if (convert_to_angstroms) {
    center *= bohr_to_ang;
  }

  result += (std::to_string(center[0]) + ' ');
  result += (std::to_string(center[1]) + ' ');
  result += std::to_string(center[2]);

  return result;
}

Atom::Atom(const KeyVal& kv) {
  auto element = kv.value<std::string>("element");
  if (element.empty())
    throw InputError("invalid element", __FILE__, __LINE__, "element");
  atomic_number_ = detail::element_to_Z(element);

  auto xyz = kv.value<std::array<double, 3>>("xyz");
  center_(0) = xyz[0];
  center_(1) = xyz[1];
  center_(2) = xyz[2];

  mass_ = kv.value<double>("mass", molecule::masses::masses.at(atomic_number_));
  charge_ = kv.value<double>("charge", static_cast<double>(atomic_number_));
}

std::ostream &operator<<(std::ostream &os, Atom const &a) {
  os << "Atom: {Z: " << a.atomic_number() << ", charge: " << a.charge()
     << ", mass: " << a.mass() << ", pos: " << a.center().transpose() << "}";
  return os;
}

}  // namespace mpqc
