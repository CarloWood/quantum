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

    qc[0] - H - S - CX(1)              - measure(0);
    qc[1] - H - T - co(1)              - measure(1);
    qc[2] - H - CX(2)                  - measure(2);
    qc[3] - H - co(2)                  - measure(3);
  }

  std::cout << "The circuit:\n";
  std::cout << qc << std::endl;

  qc.execute();
  std::cout << "Result: " << qc.result() << std::endl;
}
