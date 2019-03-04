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
  Dout(dc::notice, "H = " << H);
  Dout(dc::notice, "T = " << T);
}
