#pragma once

#include "QState.h"
#include "QMatrix.h"
#include <iosfwd>

namespace quantum {

class QuBit
{
 private:
  QState m_qstate;

  explicit QuBit(QState const& qstate) : m_qstate(qstate) { }

 public:
  QuBit(ZState state) : m_qstate(state) { }

  friend std::ostream& operator<<(std::ostream& os, QuBit const& qubit);
  friend QuBit operator*(QMatrix const& m, QuBit const& qubit);
  friend bool operator==(QuBit const& qubit1, QuBit const& qubit2) { return qubit1.m_qstate == qubit2.m_qstate; }
};

} // namespace quantum
