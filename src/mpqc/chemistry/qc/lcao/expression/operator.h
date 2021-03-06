//
// Created by Chong Peng on 10/31/15.
//

#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_OPERATOR_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_OPERATOR_H_

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <TiledArray/error.h>

namespace mpqc {

/**
 * \brief Class to represent operation used in Formula
 *
 * Dictionary of wstring to operations
 *  - "" -> Overlap
 *  - T -> Kinetic
 *  - V -> Nuclear
 *  - H -> Hcore
 *  - I -> Identity
 *  - O -> SphericalMultipole(libint::sphemultipole)
 *  - G -> Coulomb(libint::Coulomb)
 *  - R -> cGTG(libint::cGTG)
 *  - GR -> cGTGCoulomb(libint::cGTG_times_Coulomb)
 *  - R2 -> cGTG2(libint::cGTG, parameters will get squared)
 *  - dR2 -> DelcGTG2(libint::DelcGTG_square)
 *  - J -> J(Coulomb)
 *  - hJ -> hJ (H + J)
 *  - K -> K(Exchange)
 *  - K(α) -> KAlpha(Exchange for Alpha Spin)
 *  - K(β) -> KBeta(Exchange for Beta Spin)
 *  - F -> Fock
 *  - F(α) -> FockAlpha(Fock for Alpha Spin)
 *  - F(β) -> FockBeta(Fock for Beta Spin)
 *  - CADF -> CADF Fitting Coefficents
 *
 */

class Operator {
 public:
  /**
   *  Operator types
   */
  enum class Type {
    Invalid = -1,
    __first_1body_operator = 0,
    Identity = 0,
    Overlap = 1,
    Kinetic = 2,
    Nuclear = 3,
    Core = 4,
    SphericalMultipole = 5,
    __last_1body_operator = 5,
    __first_fock_operator = 32,
    J = 32,
    K = 33,
    KAlpha = 34,
    KBeta = 35,
    Fock = 36,
    FockAlpha = 37,
    FockBeta = 38,
    hJ = 39,
    __last_fock_operator = 39,
    __first_2body_operator = 127,
    CADF=127,
    Coulomb = 128,
    cGTG = 129,
    cGTG2 = 130,
    cGTGCoulomb = 131,
    DelcGTG2 = 132,
    __last_2body_operator = 132
  };

  /**
   * maps of string to operations and option
   * vice versa
   */
  static const std::unordered_map<std::wstring, Type> one_body_operation;
  static const std::unordered_map<std::wstring, Type> two_body_operation;
  static const std::unordered_map<std::wstring, Type> fock_operation;
  static const std::map<Type, std::wstring> oper_to_string;

  Operator() : type_(Type::Invalid) {}
  Operator(Operator const &) = default;
  Operator(Operator &&) = default;
  Operator &operator=(Operator const &) = default;
  Operator &operator=(Operator &&) = default;

  /**
   * Constructor
   *
   * @param operation  string of operation
   */
  Operator(std::wstring operation);

  /// return operation oper_
  const Type &type() const {
    TA_USER_ASSERT(type_ != Type::Invalid, "invalid Operator");
    return type_;
  }

  /// set operation oper_
  void set_type(const Type &type) {
    TA_USER_ASSERT(type != Type::Invalid, "invalid Operator");
    Operator::type_ = type;
  }

  /// return string that correspond to oper_
  const std::wstring oper_string() const;

  /// check if oper_ is one body operation
  bool is_onebody() const;

  /// check if oper_ is two body operation
  bool is_twobody() const;

  /// check if oper_ is fock operation
  bool is_fock() const;

  /// check if oper_ is J or K operation
  bool is_jk() const;

  /// check if oper_ is R12 operation
  bool is_r12() const;

  /// equality check by comparing oper_ and options_
  bool operator==(Operator const &other) const;

  /// equality check by comparing oper_ and options_
  bool operator!=(Operator const &other) const { return !(*this == other); }

  /// comparison by comparing oper_ and options_
  bool operator<(const Operator &other) const;

 private:
  Type type_;
};

} // namespace mpqc

#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_OPERATOR_H_
