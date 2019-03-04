#pragma once

#include <complex>
#include <cmath>

namespace quantum {

class Complex
{
 private:
  std::complex<double> m_value;

 private:
  explicit Complex(std::complex<double> value) : m_value(value) { }

 public:
  constexpr Complex(double real, double imag) : m_value(real, imag) { }
  Complex(double real) : m_value(real, 0.0) { }

  Complex& operator*=(Complex const& z) { m_value *= z.m_value; return *this; }
  Complex& operator+=(Complex const& z) { m_value += z.m_value; return *this; }

  // Inverse
  Complex operator-() const { return Complex{-m_value}; }

  // Complex conjugate.
  Complex operator*() const { return Complex{std::conj(m_value)}; }

  friend bool operator==(Complex const& z1, Complex const& z2);
  friend Complex operator*(Complex const& z1, Complex const& z2) { Complex result{z1}; result *= z2; return result; }
  friend Complex operator+(Complex const& z1, Complex const& z2) { Complex result{z1}; result += z2; return result; }

  friend std::ostream& operator<<(std::ostream& os, Complex const& z);
};

template<intmax_t Num, intmax_t Den>
struct sin {
  static constexpr double pi = std::sin(Num * M_PI / Den);
};

template<intmax_t Num, intmax_t Den>
struct cos {
  static constexpr double pi = std::cos(Num * M_PI / Den);
};

template<intmax_t Num, intmax_t Den>
struct exp {
  static constexpr Complex i_pi{cos<Num, Den>::pi, sin<Num, Den>::pi};
};

static constexpr Complex i{0, 1};

} // namespace quantum
