#pragma once

#include "Complex.h"
#include <array>

namespace quantum {

class QMatrix;

class QVector
{
 private:
  std::array<Complex, 2> m_coef;

 public:
  QVector(Complex a, Complex b) : m_coef{ a, b } { }

  Complex operator[](int i) const { return m_coef[i]; }
  friend QVector operator*(QMatrix const& m, QVector const& v);
  friend bool operator==(QVector const& v1, QVector const& v2);
};

} // namespace quantum
