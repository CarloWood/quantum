#include "sys.h"
#include "YState.h"

namespace quantum {

std::string to_string(YState state)
{
  if (state == Y0)
    return "|↻⟩";
  return "|↺⟩";
}

std::ostream& operator<<(std::ostream& os, YState state)
{
  return os << to_string(state);
}

} // namespace quantum
