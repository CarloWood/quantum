#include "sys.h"
#include "YState.h"

namespace quantum {

std::string to_string(YState state)
{
  if (state == Y0)
    return "|\u21bb\u27e9"; // "|↻⟩"
  return "|\u21ba\u27e9";   // "|↺⟩"
}

std::ostream& operator<<(std::ostream& os, YState state)
{
  return os << to_string(state);
}

} // namespace quantum
