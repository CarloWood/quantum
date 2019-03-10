#pragma once

#include "QMatrix.h"
#include <array>

namespace quantum {
namespace gates {

// Index into std::array gate.
enum gate_t
{
  X = 0,
  Y = 1,
  Z = 2,
  S = 3,
  S_inv = 4,
  T = 5,
  T_inv = 6,
  H = 7,
};

static constexpr int number_of_gates = 8;

extern std::array<QMatrix, number_of_gates> const gate;

extern QMatrix const SH;
extern QMatrix const I;

} // namespace gates

using gates::gate;

} // namespace quantum
