#include "sys.h"
#include "debug.h"
#include "Circuit.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

#if 0
  Dout(dc::notice, "H =\n" << gate[H]);
  Dout(dc::notice, "S =\n" << gate[S]);
  Dout(dc::notice, "S_inv =\n" << gate[S_inv]);
  Dout(dc::notice, "S * H =\n" << (gate[S] * gate[H]));
  Dout(dc::notice, "   SH =\n" << SH);
  Dout(dc::notice, "S * H =\n" << (gate[S] * gate[H]));
  Dout(dc::notice, "Z0 = " << QuBit(Z0));
  Dout(dc::notice, "Y0 = " << (SH * QuBit(Z0)));
#endif

  Circuit qc(3, 3);

  {
    using namespace gates;

    qc[0] - H - CX(1)             - measure(0);
    qc[1] - H - co(1) - co(2)     - measure(1);
    qc[2] - H         - CX(2) - H - measure(2);
  }

  std::cout << "The circuit:\n";
  std::cout << qc << std::endl;

  qc.execute();
  std::cout << qc.result() << std::endl;
}
