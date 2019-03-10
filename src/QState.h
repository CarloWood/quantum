#pragma once

#include "XState.h"
#include "YState.h"
#include "ZState.h"
#include "QVector.h"
#include "QMatrix.h"
#include <iosfwd>

namespace quantum {

class QState
{
 private:
  QVector m_coef;

  explicit QState(QVector const& coef) : m_coef(coef) { }

 public:
  QState(XState state);
  QState(YState state);
  QState(ZState state);

  friend std::ostream& operator<<(std::ostream& os, QState const& state);
  friend QState operator*(QMatrix const& m, QState const& qstate) { return QState{m * qstate.m_coef}; }
  friend bool operator==(QState const& qstate1, QState const& qstate2) { return qstate1.m_coef == qstate2.m_coef; }
};

} // namespace quantum
