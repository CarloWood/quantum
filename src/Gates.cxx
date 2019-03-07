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

std::array<QMatrix, number_of_gates> const gate = {{
  // X
  {
    0,  1,
    1,  0
  },

  // Y
  {
    0, -i,
    i,  0
  },

  // Z
  {
    1,  0,
    0, -1
  },

  // S = T^2
  {
    1,  0,
    0,  i
  },

  // S_inv
  {
    1,  0,
    0, -i
  },

  // T
  {
    1,  0,
    0,  exp<1,4>::i_pi
  },

  // T_inv
  {
    1,  0,
    0,  exp<-1,4>::i_pi
  },

  // H
  {
    cos<1,4>::pi, cos<-1,4>::pi,
    sin<1,4>::pi, sin<-1,4>::pi
  }
}};

// (SH)⁻¹
QMatrix const HS_inv = gate[H] * gate[S_inv];

// Identity matrix.
QMatrix const I{
  1, 0,
  0, 1
};

} // namespace gates

} // namespace quantum
