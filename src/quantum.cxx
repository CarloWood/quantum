#include "sys.h"
#include "debug.h"
#include "Circuit.h"

using namespace quantum;

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  Circuit qc(2, 2);
  Link l1;

  {
    using namespace gates;

    qc[0] << H << CX(l1) << measure(0);
    //             ^
    //             |
    qc[1] << H << co(l1) << measure(1);
  }

  std::cout << qc.result() << std::endl;
}
