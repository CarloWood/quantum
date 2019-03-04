#include "QState.h"
#include "QMatrix.h"
#include <iostream>

namespace quantum {

QState::QState(ZState state) : m_coef(state == Z0 ? QVector{1, 0} : QVector{0, 1})
{
}

std::ostream& operator<<(std::ostream& os, QState const& state)
{
  if (state == Z0)
    os << Z0;
  else if (state == Z1)
    os << Z1;
  else
    os << "Unknown state " << state.m_coef[0] << " |0> + " << state.m_coef[1] << " |1>";
  return os;
}

QState operator*(QMatrix const& m, QState const& qstate)
{
  return QState{m * qstate.m_coef};
}

bool operator==(QState const& qstate1, QState const& qstate2)
{
  return qstate1.m_coef == qstate2.m_coef;
}

} // namespace quantum
