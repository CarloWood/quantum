#include "sys.h"
#include "debug.h"
#include "QuBit.h"
#include "Gates.h"
#include <random>

using namespace quantum;

void apply_to(QuBit& qb, int gate)
{
  switch (gate)
  {
    case 0:
      qb = X * qb;
      break;
    case 1:
      qb = H * qb;
      break;
    case 2:
      qb = T * qb;
      break;
  }
}

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, 2);

  QuBit Yqb(Z0);
  QuBit Zqb(Z0);
  QuBit Sqb(Z0);

  Yqb = Y * Yqb;
  Zqb = Z * Zqb;
  Sqb = S * Sqb;

  QuBit qb0(Z0);

  for (int i = 0; i < 10000; ++i)
  {
    int gate = uni(rng);
    apply_to(qb0, gate);
    Dout(dc::notice, qb0);
  }
}
