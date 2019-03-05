#pragma once

#include "QuBitField.h"
#include "debug.h"
#include <iosfwd>

namespace quantum {

class QVector;

class QMatrix
{
 private:
  QuBitField m_a_00;
  QuBitField m_a_01;
  QuBitField m_a_10;
  QuBitField m_a_11;

 public:
  QMatrix(QuBitField a_00, QuBitField a_01,
          QuBitField a_10, QuBitField a_11) :
    m_a_00(a_00), m_a_01(a_01), m_a_10(a_10), m_a_11(a_11)
  {
    Dout(dc::notice, "Calling QMatrix(" << a_00 << ", " << a_01 << ", " << a_10 << ", " << a_11 << ")");
  }

 friend std::ostream& operator<<(std::ostream& os, QMatrix const&);
 friend QVector operator*(QMatrix const& m, QVector const& v);
 friend QMatrix operator*(QMatrix const& m1, QMatrix const& m2);
};

} // namespace quantum
