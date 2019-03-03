#include "sys.h"
#include "QMatrix.h"
#include <iostream>

namespace quantum {

std::ostream& operator<<(std::ostream& os, QMatrix const& m)
{
  os << "((" << m.m_a_00 << ", " << m.m_a_01 << "), (" << m.m_a_10 << ", " << m.m_a_11 << "))";
  return os;
}

QMatrix operator*(QMatrix const& m1, QMatrix const& m2)
{
  return QMatrix{ m1.m_a_00 * m2.m_a_00 + m1.m_a_01 * m2.m_a_10, m1.m_a_00 * m2.m_a_01 + m1.m_a_01 * m2.m_a_11,
                 m1.m_a_10 * m2.m_a_00 + m1.m_a_11 * m2.m_a_10, m1.m_a_10 * m2.m_a_01 + m1.m_a_11 * m2.m_a_11 };
}

} // namespace quantum
