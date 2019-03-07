#pragma once

#include "QMatrix.h"
#include <cmath>

namespace quantum {

namespace gates {

using gate_t = int;

static constexpr gate_t X = 0;
static constexpr gate_t Y = 1;
static constexpr gate_t Z = 2;
static constexpr gate_t S = 3;
static constexpr gate_t S_inv = 4;
static constexpr gate_t T = 5;
static constexpr gate_t T_inv = 6;
static constexpr gate_t H = 7;
static constexpr gate_t number_of_gates = 8;

extern std::array<QMatrix, number_of_gates> const gate;

extern QMatrix const HS_inv;
extern QMatrix const I;

} // namespace gates

using gates::gate;

} // namespace quantum
