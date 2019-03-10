#include "sys.h"
#include "Gates.h"

namespace quantum {

namespace gates {

// Instantiation of specialized static constants.
QuBitField const sin<1, 4>::pi = sqrt_half;
QuBitField const sin<-1, 4>::pi = -sqrt_half;
QuBitField const cos<1, 4>::pi = sqrt_half;
QuBitField const cos<-1, 4>::pi = sqrt_half;
QuBitField const exp<1, 4>::i_pi{0, 0, 1, 1};   // (1 + i)·√½
QuBitField const exp<-1, 4>::i_pi{0, 0, 1, -1}; // (1 - i)·√½

namespace {

QMatrix::Scalar I_init[4] =
  {
    1, 0,
    0, 1
  };

QMatrix::Scalar X_init[4] =
  {
    0,  1,
    1,  0
  };

QMatrix::Scalar Y_init[4] =
  {
    0, -i,
    i,  0
  };

QMatrix::Scalar Z_init[4] =
  {
    1,  0,
    0, -1
  };

QMatrix::Scalar S_init[4] = // T^2
  {
    1,  0,
    0,  i
  };

QMatrix::Scalar S_inv_init[4] = // T^6
  {
    1,  0,
    0, -i
  };

QMatrix::Scalar T_init[4] =
  {
    1,  0,
    0,  exp<1,4>::i_pi
  };

QMatrix::Scalar T_inv_init[4] = // T^7
  {
    1,  0,
    0,  exp<-1,4>::i_pi
  };

QMatrix::Scalar H_init[4] =
  {
    cos<1,4>::pi, cos<-1,4>::pi,
    sin<1,4>::pi, sin<-1,4>::pi
  };

} // namespace

std::array<QMatrix, number_of_gates> const gate = {
  QMatrix{X_init},
  QMatrix{Y_init},
  QMatrix{Z_init},
  QMatrix{S_init},
  QMatrix{S_inv_init},
  QMatrix{T_init},
  QMatrix{T_inv_init},
  QMatrix{H_init}
};

// Only needed in QState.cxx, but defined here to avoid static initialization fiasco.

// (SH)⁻¹
QMatrix const SH = gate[S] * gate[H];

// Identity matrix.
QMatrix const I{I_init};

} // namespace gates

} // namespace quantum
