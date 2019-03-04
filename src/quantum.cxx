#include "sys.h"
#include "debug.h"
#include "QuBit.h"
#include "Gates.h"

using namespace quantum;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  QuBit qb0(Z0);
  QuBit qb1(Z1);

  Dout(dc::notice, qb0);
  Dout(dc::notice, qb1);

  qb0 = H * qb0;
  qb1 = H * qb1;

  Dout(dc::notice, "H " << Z0 << " = " << qb0 << " (should be " << X0 << ')');
  Dout(dc::notice, "H " << Z1 << " = " << qb1 << " (should be " << X1 << ')');

  qb0 = S * qb0;
  qb1 = S * qb1;

  Dout(dc::notice, "S H " << Z0 << " = " << qb0 << " (should be " << Y0 << ')');
  Dout(dc::notice, "S H " << Z1 << " = " << qb1 << " (should be " << Y1 << ')');
}
