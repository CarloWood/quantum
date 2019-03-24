#include "sys.h"
#include "QState.h"
#include "QMatrix.h"
#include "Gates.h"
#include <iostream>

namespace quantum {

using namespace gates;

QState::QState(XState state) :
  m_coef(state == X0 ? gate[H].col(0)           // H |0⟩ = |+⟩
                     : gate[H].col(1))          // H |1⟩ = |-⟩
{
}

QState::QState(YState state) :
  m_coef(state == Y0 ? SH.col(0)                // S H |0⟩ = |↻⟩
                     : SH.col(1))               // S H |1⟩ = |↺⟩
{
}

QState::QState(ZState state) :
  m_coef(state == Z0 ? I.col(0)                 // I |0⟩ = |0⟩
                     : I.col(1))                // I |1⟩ = |1⟩
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
    os << state.m_coef[0] << " |0\u27e9 "; // " |0⟩ "
    std::stringstream ss;
    ss << state.m_coef[1];
    std::string s = ss.str();
    if (s[0] == '-')
      os << "- " << s.substr(1);
    else
      os << "+ " << s;
    os << " |1\u27e9"; // " |1⟩"
    if (print_brackets)
      os << ']';
  }
  return os;
}

} // namespace quantum
