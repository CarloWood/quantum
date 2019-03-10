#include "sys.h"
#include "QuBit.h"
#include <iostream>

namespace quantum {

std::ostream& operator<<(std::ostream& os, QuBit const& qubit)
{
  os << qubit.m_qstate;
  return os;
}

QuBit operator*(QMatrix const& m, QuBit const& qubit)
{
  return QuBit{m * qubit.m_qstate};
}

} // namespace quantum
