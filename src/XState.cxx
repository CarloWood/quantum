#include "sys.h"
#include "XState.h"

namespace quantum {

std::string to_string(XState state)
{
  if (state == X0)
    return "|+\u27e9";    // "|+⟩"
  return "|\u2212\u27e9"; // "|−⟩"
}

std::ostream& operator<<(std::ostream& os, XState state)
{
  return os << to_string(state);
}

} // namespace quantum
