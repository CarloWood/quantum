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
  Complex(double real, double imag) : m_value(real, imag) { }
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

static Complex const one{1, 0};
static Complex const zero{0, 0};
static Complex const one_i{0, 1};

} // namespace quantum
