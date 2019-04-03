#include "sys.h"
#include "debug.h"
#include "EntangledState.h"
#include "State.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit q(2, 2);
  {
    using namespace gates;

    q[1] - H - co(0) - H - measure(0);
    q[0]     - CX(0) - S - H - T_inv - H - measure(1);
  }

  std::cout << "The circuit:\n";
  std::cout << q << std::endl;
  q.execute();
  std::shared_ptr<State> state = q.state();
  std::cout << "\nResult: " << *state << '\n' << std::endl;
}
