#pragma once

#include "formula.h"
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
class QuBitField : public formula::Sum
{
 private:
  mpq_rational nr_;     // Non-root Real: k.
  mpq_rational ni_;     // Non-root Imaginary: l.
  mpq_rational rr_;     // Root Real: m.
  mpq_rational ri_;     // Root Imaginary: n.

  QuBitField(double) {}

 public:
  QuBitField() : nr_(0), ni_(0), rr_(0), ri_(0) { }
  QuBitField(int nr) : nr_(nr), ni_(0), rr_(0), ri_(0) { }
  QuBitField(mpq_rational nr) : nr_(nr), ni_(0), rr_(0), ri_(0) { }
  QuBitField(mpq_rational nr, mpq_rational ni, mpq_rational rr, mpq_rational ri) : nr_(nr), ni_(ni), rr_(rr), ri_(ri) { }
  QuBitField(QuBitField const& v) : nr_(v.nr_), ni_(v.ni_), rr_(v.rr_), ri_(v.ri_) { }

  QuBitField& operator+=(QuBitField const& v) { nr_ += v.nr_; ni_ += v.ni_; rr_ += v.rr_; ri_ += v.ri_; return *this; }
  QuBitField& operator-=(QuBitField const& v) { nr_ -= v.nr_; ni_ -= v.ni_; rr_ -= v.rr_; ri_ -= v.ri_; return *this; }
  QuBitField& operator*=(QuBitField const& v) { QuBitField self(*this); *this = self * v; return *this; }
  QuBitField operator-() const { return QuBitField(-nr_, -ni_, -rr_, -ri_); }

//  std::string to_string(bool need_parens = false) const;

  friend QuBitField operator+(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result += v2; return result; }
  friend QuBitField operator-(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result -= v2; return result; }
  friend QuBitField operator*(QuBitField const& v1, QuBitField const& v2);
  friend bool operator==(QuBitField const& v1, QuBitField const& v2) { return v1.nr_ == v2.nr_ && v1.ni_ == v2.ni_ && v1.rr_ == v2.rr_ && v1.ri_ == v2.ri_; }
  friend bool operator!=(QuBitField const& v1, QuBitField const& v2) { return v1.nr_ != v2.nr_ || v1.ni_ != v2.ni_ || v1.rr_ != v2.rr_ || v1.ri_ != v2.ri_; }

 public:
  // For printing (override virtual functions of formula::Sum).
  bool starts_with_a_minus() const override { return nr_ < 0 || (nr_ == 0 && (ni_ < 0 || (ni_ == 0 && (rr_ < 0 || (rr_ == 0 && ri_ < 0))))); }
  bool has_multiple_terms() const override { return (nr_ != 0 ? 1 : 0) + (ni_ != 0 ? 1 : 0) + (rr_ != 0 || ri_ != 0 ? 1 : 0) > 1; }
  bool is_zero() const override { return nr_ == 0 && ni_ == 0 && rr_ == 0 && ri_ == 0; }
  bool is_unity() const override { return (nr_ == 1 || nr_ == -1) && ni_ == 0 && rr_ == 0 && ri_ == 0; }
  void print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const override;

#if 0
 public:
  // Implement virtual methods of Sum.
  bool is_one() const override { return nr_ == 1 && ni_ == 0 && rr_ == 0 && ri_ == 0; }
  char const* plus_string() const { return " + "; }
  char const* minus_string() const { return " - "; }
#endif
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
#include <Eigen/Core>

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
