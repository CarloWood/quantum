#include "QMatrix.h"
#include "Complex.h"

namespace quantum {

static double const sqrt_0_5 = 0.70710678118;

static QMatrix const H{
  sqrt_0_5,  sqrt_0_5,
  sqrt_0_5, -sqrt_0_5
};

static QMatrix const T{
  one,  zero,
  zero, Complex{sqrt_0_5, sqrt_0_5}
};

static QMatrix const X{
  zero, one,
  one,  zero
};

static QMatrix const Y{
  zero, -one_i,
  one_i, zero
};

static QMatrix const Z{
  one, zero,
  zero, -one
};

static QMatrix const S = T * T;
static QMatrix const Sdg = Z * S;
static QMatrix const Tdg = Sdg * T;

} // namespace quantum
