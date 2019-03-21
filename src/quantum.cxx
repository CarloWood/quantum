#include "sys.h"
#include "debug.h"
#include "EntangledState.h"
#include "State.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit qc(7, 0);
  {
    using namespace gates;

    qc[5] - H - S     - co(1)         - co(2);
    qc[3] - H - S_inv - CX(1) - H - T - CX(2) - T - H - S - X;
  }

  std::cout << "The circuit:\n";
  std::cout << qc << std::endl;
  qc.execute();
  std::shared_ptr<State> state = qc.state();
  std::cout << "\nResult: " << *state << '\n' << std::endl;
}
