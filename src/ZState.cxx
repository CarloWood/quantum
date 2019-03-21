#include "sys.h"
#include "ZState.h"

namespace quantum {

std::string to_string(ZState state)
{
  if (state == Z0)
    return "|0\u27e9"; // "|0⟩"
  return "|1\u27e9";   // "|1⟩"
}

std::ostream& operator<<(std::ostream& os, ZState state)
{
  return os << to_string(state);
}

} // namespace quantum
