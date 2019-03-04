#include "sys.h"
#include "XState.h"

namespace quantum {

std::string to_string(XState state)
{
  if (state == X0)
    return "|+⟩";
  return "|−⟩";
}

std::ostream& operator<<(std::ostream& os, XState state)
{
  return os << to_string(state);
}

} // namespace quantum
