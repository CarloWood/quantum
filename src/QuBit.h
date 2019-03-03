#pragma once

#include "QState.h"
#include <iosfwd>

namespace quantum {

class QMatrix;

class QuBit
{
 private:
  QState m_qstate;

  explicit QuBit(QState const& qstate) : m_qstate(qstate) { }

 public:
  QuBit(ZState state) : m_qstate(state) { }

  friend std::ostream& operator<<(std::ostream& os, QuBit const& qubit);
  friend QuBit operator*(QMatrix const& m, QuBit const& qubit);
};

} // namespace quantum
