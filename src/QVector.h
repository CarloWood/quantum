#pragma once

#include "QuBitField.h"
#include <array>

namespace quantum {

class QMatrix;

class QVector
{
 private:
  std::array<QuBitField, 2> m_coef;

 public:
  QVector(QuBitField a, QuBitField b) : m_coef{ a, b } { }

  QuBitField& operator[](int i) { return m_coef[i]; }
  QuBitField operator[](int i) const { return m_coef[i]; }
  friend QVector operator*(QMatrix const& m, QVector const& v);
  friend bool operator==(QVector const& v1, QVector const& v2);
};

} // namespace quantum
