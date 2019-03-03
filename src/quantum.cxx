#include "sys.h"
#include "debug.h"
#include "QuBit.h"
#include "Gates.h"

using namespace quantum;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  QuBit qb(Z0);

  qb = H * qb;
  qb = S * qb;
  qb = T * qb;
  qb = H * qb;
  qb = Sdg * qb;

  Dout(dc::notice, qb);

  qb = H * qb;
  Dout(dc::notice, qb);
}
