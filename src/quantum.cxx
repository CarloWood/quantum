#include "sys.h"
#include "debug.h"
#include "Circuit.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Dout(dc::notice, "H =\n" << gate[H]);
  Dout(dc::notice, "S =\n" << gate[S]);
  Dout(dc::notice, "S_inv =\n" << gate[S_inv]);
  Dout(dc::notice, "S * H =\n" << (gate[S] * gate[H]));
  Dout(dc::notice, "   SH =\n" << SH);
  Dout(dc::notice, "S * H =\n" << (gate[S] * gate[H]));
  Dout(dc::notice, "Z0 = " << QuBit(Z0));
  Dout(dc::notice, "Y0 = " << (SH * QuBit(Z0)));

#if 0
  Circuit qc(2, 2);
  Link l1(1);

  {
    using namespace gates;

    qc[0] << H << CX(l1) << measure(0);
    //             ^
    //             |
    qc[1] << H << co(l1) << measure(1);
  }

  qc.execute();
  std::cout << qc.result() << std::endl;
#endif
}
