#pragma once

#include "QuBitField.h"

namespace quantum {

// Type for NxN matrices with N > 2. Use QMatrix for 2x2 matrices.
using QMatrixX = Eigen::Matrix<QuBitField, Eigen::Dynamic, Eigen::Dynamic>;

} // namespace quantum
