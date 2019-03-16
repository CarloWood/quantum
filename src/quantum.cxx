#include "sys.h"
#include "debug.h"
#include "EntangledState.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit qc(4, 4);

  {
    using namespace gates;

    qc[0] - H - S - CX(1) - CX(3) - S_inv - H;
    qc[1] - H - T - co(1) - co(3) - T_inv - H;
    qc[2] - H - CX(2) - H;
    qc[3] - H - co(2) - H;
  }

  std::cout << "The circuit:\n";
  std::cout << qc << std::endl;

  qc.execute();
  std::cout << "Result: " << qc.result() << std::endl;
}
