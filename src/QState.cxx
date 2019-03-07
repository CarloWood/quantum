#include "sys.h"
#include "QState.h"
#include "QMatrix.h"
#include "Gates.h"
#include <iostream>

namespace quantum {

using namespace gates;

QState::QState(XState state) :
  m_coef(state == X0 ? gate[H].column(0)        // H |+⟩ = |0⟩
                     : gate[H].column(1))       // H |-⟩ = |1⟩
{
}

QState::QState(YState state) :
  m_coef(state == Y0 ? HS_inv.column(0)         // H S⁻¹ |↻⟩ = |0⟩
                     : HS_inv.column(1))        // H S⁻¹ |↺⟩ = |1⟩
{
}

QState::QState(ZState state) :
  m_coef(state == Z0 ? I.column(0)              // I |0⟩ = |0⟩
                     : I.column(1))             // I |1⟩ = |1⟩
{
}

std::ostream& operator<<(std::ostream& os, QState const& state)
{
  if (state == Z0)
    os << Z0;
  else if (state == Z1)
    os << Z1;
  else
  {
    bool print_brackets = true;
    if (state == X0)
      os << X0;
    else if (state == X1)
      os << X1;
    else if (state == Y0)
      os << Y0;
    else if (state == Y1)
      os << Y1;
    else
      print_brackets = false;
    if (print_brackets)
      os << " [";
    os << state.m_coef[0].to_string(true) << " |0⟩ ";
    std::stringstream ss;
    ss << state.m_coef[1].to_string(true);
    std::string s = ss.str();
    if (s[0] == '-')
      os << "- " << s.substr(1);
    else
      os << "+ " << s;
    os << " |1⟩";
    if (print_brackets)
      os << ']';
  }
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
