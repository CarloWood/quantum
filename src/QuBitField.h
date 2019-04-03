#pragma once

#include "formula.h"
#include <Eigen/Core>
#include <boost/multiprecision/gmp.hpp>
#include <string>

// Forward declarations.
namespace quantum {

using namespace boost::multiprecision;

// Just a helper class for printing QuBitField.
struct RationalsComplex
{
  mpq_rational m_real;
  mpq_rational m_imag;

  RationalsComplex(mpq_rational real, mpq_rational imag) : m_real(real), m_imag(imag) { }

  bool is_unity() const { return m_imag == 0 && (m_real == 1 || m_real == -1); }
  bool starts_with_a_minus() const { return m_real < 0 || (m_real == 0 && m_imag < 0); }
};

class QuBitField;

} // namespace quantum

namespace formula {

// Specialization.

template<>
void print_formula_on(quantum::RationalsComplex const& number, std::ostream& os, bool negate_all_terms, bool is_factor);

} // namespace formula

namespace quantum {

// This class represents the numbers ℚ[i, 1/√2] = { (k + l·i) + (m + n·i)·√½ | k,l,m,n ∈ ℚ }
class QuBitField : public formula::Sum<Eigen::Matrix<mpq_rational, 4, 1>>
{
  using base_type = formula::Sum<Eigen::Matrix<mpq_rational, 4, 1>>;

 private:
  static constexpr int nr_ = 0; // Non-root Real: k.
  static constexpr int ni_ = 1; // Non-root Imaginary: l.
  static constexpr int rr_ = 2; // Root Real: m.
  static constexpr int ri_ = 3; // Root Imaginary: n.

 public:
  using base_type::Sum;
  QuBitField() : base_type{{0, 0, 0, 0}} { }
  QuBitField(int nr) : base_type{{nr, 0, 0, 0}} { }
  QuBitField(mpq_rational nr) : base_type{{nr, 0, 0, 0}} { }
  QuBitField(mpq_rational nr, mpq_rational ni, mpq_rational rr, mpq_rational ri) : base_type{{ nr, ni, rr, ri}} { }
  QuBitField(QuBitField const& v) : base_type(v.m_sum) { }

  QuBitField& operator+=(QuBitField const& v) { m_sum += v.m_sum; return *this; }
  QuBitField& operator-=(QuBitField const& v) { m_sum -= v.m_sum; return *this; }
  QuBitField& operator*=(QuBitField const& v) { QuBitField self(*this); *this = self * v; return *this; }
  QuBitField operator-() const { return base_type::container_type(-m_sum); }

  friend QuBitField operator+(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result += v2; return result; }
  friend QuBitField operator-(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result -= v2; return result; }
  friend QuBitField operator*(QuBitField const& v1, QuBitField const& v2);
  friend bool operator==(QuBitField const& v1, QuBitField const& v2) { return v1.m_sum == v2.m_sum; }
  friend bool operator!=(QuBitField const& v1, QuBitField const& v2) { return v1.m_sum != v2.m_sum; }

  QuBitField conjugate() const { return { m_sum[0], -m_sum[1], m_sum[2], -m_sum[3] }; }

 public:
  // For printing (override virtual functions of formula::Sum).
  bool starts_with_a_minus() const override { return m_sum[nr_] < 0 || (m_sum[nr_] == 0 && (m_sum[ni_] < 0 || (m_sum[ni_] == 0 && (m_sum[rr_] < 0 || (m_sum[rr_] == 0 && m_sum[ri_] < 0))))); }
  bool has_multiple_terms() const override { return (m_sum[nr_] != 0 ? 1 : 0) + (m_sum[ni_] != 0 ? 1 : 0) + (m_sum[rr_] != 0 || m_sum[ri_] != 0 ? 1 : 0) > 1; }
  bool is_zero() const override { return m_sum[nr_] == 0 && m_sum[ni_] == 0 && m_sum[rr_] == 0 && m_sum[ri_] == 0; }
  bool is_unity() const override { return (m_sum[nr_] == 1 || m_sum[nr_] == -1) && m_sum[ni_] == 0 && m_sum[rr_] == 0 && m_sum[ri_] == 0; }
  void print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const override;
};

namespace gates {

static QuBitField const i{0, 1, 0, 0};
static QuBitField const sqrt_half{0, 0, 1, 0};

template<intmax_t Num, intmax_t Den>
struct sin {
};

template<>
struct sin<1, 4> {
  static const QuBitField pi;
};

template<>
struct sin<-1, 4> {
  static QuBitField const pi;
};

template<intmax_t Num, intmax_t Den>
struct cos {
};

template<>
struct cos<1, 4> {
  static QuBitField const pi;
};

template<>
struct cos<-1, 4> {
  static QuBitField const pi;
};

template<intmax_t Num, intmax_t Den>
struct exp {
};

template<>
struct exp<1, 4> {
  static QuBitField const i_pi;
};

template<>
struct exp<-1, 4> {
  static QuBitField const i_pi;
};

} // namespace gates

} // namespace quantum

// Add support for libeigen3. See https://eigen.tuxfamily.org/dox/TopicCustomizing_CustomScalar.html

namespace Eigen {

template<> struct NumTraits<quantum::QuBitField> : GenericNumTraits<quantum::QuBitField>
{
  typedef quantum::QuBitField Real;
  typedef quantum::QuBitField NonInteger;
  typedef quantum::QuBitField Nested;

  static inline Real epsilon() { return 0; }
  static inline Real dummy_precision() { return 0; }
  static inline int digits10() { return 0; }

  enum {
    IsInteger = 0,
    IsSigned = 1,
    IsComplex = 0,
    RequireInitialization = 1,
    ReadCost = 24,
    AddCost = 600,
    MulCost = 400
  };
};

} // namespace Eigen
