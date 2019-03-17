#include "sys.h"
#include "debug.h"
#include "EntangledState.h"
#include "State.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit qc(2, 0);
  {
    using namespace gates;

    qc[0] - H - T;
    qc[1] - H - T_inv;
  }

  std::cout << "The circuit:\n";
  std::cout << qc << std::endl;
  qc.execute();
  std::cout << "\nResult: " << qc.result() << '\n' << std::endl;

  qc.reset(2, 0);
  {
    using namespace gates;

    qc[0] - H - S - T  - co(1);
    qc[1] - H - T_inv  - CX(1);
  }

  std::cout << "Circuit with controlled-NOT gate:\n";
  std::cout << qc << std::endl;
  qc.execute();
  std::cout << "\nResult: " << qc.result() << '\n' << std::endl;

  qc.reset(2, 0);
  {
    using namespace gates;

    qc[0] - H - T_inv  - H - co(1) - H;
    qc[1] - H - S - T  - H - CX(1) - H;
  }

  std::cout << "Circuit with \"inverse\" controlled-NOT gate:\n";
  std::cout << qc << std::endl;
  qc.execute();
  auto result1 = qc.state();
  std::cout << "\nState: " << *result1 << '\n' << std::endl;

  qc.reset(2, 0);
  {
    using namespace gates;

    qc[0] - H - T_inv  - CX(1);
    qc[1] - H - S - T  - co(1);
  }

  std::cout << "Circuit with actual inversed controlled-NOT gate:\n";
  std::cout << qc << std::endl;
  qc.execute();
  auto result2 = qc.state();
  std::cout << "\nResult: " << *result2 << '\n' << std::endl;

  assert(*result1 == *result2);
}
