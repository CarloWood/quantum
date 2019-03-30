#include "sys.h"
#include "debug.h"
#include "EntangledState.h"
#include "State.h"

using namespace quantum;
using namespace gates;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit q(3, 3);
  {
    using namespace gates;

    q[2] - H                      - co(3);
    q[1] - H - co(1)          - S - CX(3);
    q[0] - X - CX(1) - measure(0);
  }

  std::cout << "The circuit:\n";
  std::cout << q << std::endl;
  q.execute();
  std::shared_ptr<State> state = q.state();
  std::cout << "\nResult: " << *state << '\n' << std::endl;
}
