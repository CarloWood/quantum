#include "sys.h"
#include "Gates.h"

namespace quantum {

// Instantiation of specialized static constants.
QuBitField const sin<1, 4>::pi = sqrt_half;
QuBitField const sin<-1, 4>::pi = -sqrt_half;
QuBitField const cos<1, 4>::pi = sqrt_half;
QuBitField const cos<-1, 4>::pi = sqrt_half;
QuBitField const exp<1, 4>::i_pi{0, 0, 1, 1};   // (1 + i)·√½
QuBitField const exp<-1, 4>::i_pi{0, 0, 1, -1}; // (1 - i)·√½

QMatrix const X{
  0,  1,
  1,  0
};

QMatrix const Y{
  0, -i,
  i,  0
};

QMatrix const Z{
  1,  0,
  0, -1
};

QMatrix const S{ // T^2
  1,  0,
  0,  i
};

QMatrix const Sdg{ // S^-1
  1,  0,
  0, -i
};

QMatrix const T{
  1,  0,
  0,  exp<1,4>::i_pi
};

QMatrix const Tdg{ // T^-1
  1,  0,
  0,  exp<-1,4>::i_pi
};

QMatrix const H(
  cos<1,4>::pi, cos<-1,4>::pi,
  sin<1,4>::pi, sin<-1,4>::pi
);

} // namespace quantum
