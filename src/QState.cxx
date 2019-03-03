#include "QState.h"
#include "QMatrix.h"
#include <iostream>

namespace quantum {

QState::QState(ZState state) : m_coef(state == Z0 ? QVector{one, zero} : QVector{zero, one})
{
}

std::ostream& operator<<(std::ostream& os, QState const& state)
{
  if (state.m_coef[0] == one)
    os << Z0;
  else if (state.m_coef[1] == one)
    os << Z1;
  else
    os << "Unknown state " << state.m_coef[0] << " |0> + " << state.m_coef[1] << " |1>";
  return os;
}

QState operator*(QMatrix const& m, QState const& qstate)
{
  return QState{m * qstate.m_coef};
}

} // namespace quantum
