#include "sys.h"
#include "QVector.h"
#include "QMatrix.h"

namespace quantum {

QVector operator*(QMatrix const& m, QVector const& v)
{
  return QVector{ m.m_a_00 * v.m_coef[0] + m.m_a_01 * v.m_coef[1],
                  m.m_a_10 * v.m_coef[0] + m.m_a_11 * v.m_coef[1] };
}

} // namespace quantum
