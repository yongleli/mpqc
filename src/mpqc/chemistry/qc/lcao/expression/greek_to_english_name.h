//
// Created by Chong Peng on 1/25/16.
//

#ifndef MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_GREEK_TO_ENGLISH_NAME_H_
#define MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_GREEK_TO_ENGLISH_NAME_H_

#include <string>
#include <unordered_map>

namespace mpqc {
const std::unordered_map<wchar_t, std::string> greek_to_english_name = {
    {L'Α', "ALPHA"},   {L'Β', "BETA"},  {L'Γ', "GAMMA"},   {L'Δ', "DELTA"},
    {L'Ε', "EPSILON"}, {L'Ζ', "ZETA"},  {L'Η', "ETA"},     {L'Θ', "THETA"},
    {L'Ι', "IOTA"},    {L'Κ', "KAPPA"}, {L'Λ', "LAMBDA"},   {L'Μ', "MU"},
    {L'Ν', "NU"},      {L'Ξ', "XI"},    {L'Ο', "OMICRON"}, {L'Π', "PI"},
    {L'Ρ', "RHO"},     {L'Σ', "SIGMA"}, {L'Τ', "TAU"},     {L'Υ', "UPSILON"},
    {L'Φ', "PHI"},     {L'Χ', "CHI"},   {L'Ψ', "PSI"},     {L'Ω', "OMEGA"},
    {L'α', "alpha"},   {L'β', "beta"},  {L'γ', "gamma"},   {L'δ', "delta"},
    {L'ε', "epsilon"}, {L'ζ', "zeta"},  {L'η', "eta"},     {L'θ', "theta"},
    {L'ι', "iota"},    {L'κ', "kappa"}, {L'λ', "lambda"},   {L'μ', "mu"},
    {L'ν', "nu"},      {L'ξ', "xi"},    {L'ο', "omicron"}, {L'π', "pi"},
    {L'ρ', "rho"},     {L'σ', "sigma"}, {L'τ', "tau"},     {L'υ', "upsilon"},
    {L'φ', "phi"},     {L'χ', "chi"},   {L'ψ', "psi"},     {L'ω', "omega"}};
}

#endif  // MPQC4_SRC_MPQC_CHEMISTRY_QC_EXPRESSION_GREEK_TO_ENGLISH_NAME_H_
