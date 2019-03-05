#include "sys.h"
#include "QuBitField.h"
#include "debug.h"

using namespace quantum;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  mpq_rational th(2, 3);
  for (int k = -1; k <= 1; ++k)
    for (int l = -1; l <= 1; ++l)
      for (int m = -1; m <= 1; ++m)
        for (int n = -1; n <= 1; ++n)
        {
          QuBitField v(k * th, l * th, m * th, n * th);
          Dout(dc::notice, v);
        }
}
