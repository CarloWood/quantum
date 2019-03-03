#pragma once

#include "Complex.h"
#include <iosfwd>

namespace quantum {

class QVector;

class QMatrix
{
 private:
  Complex m_a_00;
  Complex m_a_01;
  Complex m_a_10;
  Complex m_a_11;

 public:
  QMatrix(Complex a_00, Complex a_01,
         Complex a_10, Complex a_11) :
    m_a_00(a_00), m_a_01(a_01), m_a_10(a_10), m_a_11(a_11) { }

 friend std::ostream& operator<<(std::ostream& os, QMatrix const&);
 friend QVector operator*(QMatrix const& m, QVector const& v);
 friend QMatrix operator*(QMatrix const& m1, QMatrix const& m2);
};

} // namespace quantum
