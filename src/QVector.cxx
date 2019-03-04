#include "sys.h"
#include "QVector.h"
#include "QMatrix.h"

namespace quantum {

QVector operator*(QMatrix const& m, QVector const& v)
{
  return QVector{ m.m_a_00 * v.m_coef[0] + m.m_a_01 * v.m_coef[1],
                  m.m_a_10 * v.m_coef[0] + m.m_a_11 * v.m_coef[1] };
}

bool operator==(QVector const& v1, QVector const& v2)
{
  return v1[0] == v2[0] && v1[1] == v2[1];
}

} // namespace quantum
