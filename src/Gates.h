#include "QMatrix.h"
#include "Complex.h"
#include <cmath>

namespace quantum {

static QMatrix const H{
  cos<1,4>::pi, cos<-1,4>::pi,
  sin<1,4>::pi, sin<-1,4>::pi
};

static QMatrix const T{
  1,  0,
  0,  exp<1,4>::i_pi
};

static QMatrix const X{
  0,  1,
  1,  0
};

static QMatrix const Y{
  0, -i,
  i,  0
};

static QMatrix const Z{
  1,  0,
  0, -1
};

static QMatrix const S{ // T^2
  1,  0,
  0,  i
};

static QMatrix const Sdg{ // S^-1
  1,  0,
  0, -i
};

static QMatrix const Tdg{ // T^-1
  1,  0,
  0,  exp<-1,4>::i_pi
};

} // namespace quantum
