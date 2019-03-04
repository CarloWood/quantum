#pragma once

#include "XState.h"
#include "YState.h"
#include "ZState.h"
#include "QVector.h"
#include <iosfwd>

namespace quantum {

class QMatrix;

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
  friend QState operator*(QMatrix const& m, QState const& qstate);
  friend bool operator==(QState const& qstate1, QState const& qstate2);
};

} // namespace quantum
