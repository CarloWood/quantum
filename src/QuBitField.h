#pragma once

#include <boost/multiprecision/gmp.hpp>
#include <string>
#include <iosfwd>

namespace quantum {

using namespace boost::multiprecision;

// This class represents the numbers ℚ[i, 1/√2] = { (k + l·i) + (m + n·i)·√½ | k,l,m,n ∈ ℚ }
class QuBitField
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

  std::string to_string(bool need_parens = false) const;

  friend QuBitField operator+(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result += v2; return result; }
  friend QuBitField operator-(QuBitField const& v1, QuBitField const& v2) { QuBitField result(v1); result -= v2; return result; }
  friend QuBitField operator*(QuBitField const& v1, QuBitField const& v2);
  friend bool operator==(QuBitField const& v1, QuBitField const& v2) { return v1.nr_ == v2.nr_ && v1.ni_ == v2.ni_ && v1.rr_ == v2.rr_ && v1.ri_ == v2.ri_; }
  friend bool operator!=(QuBitField const& v1, QuBitField const& v2) { return v1.nr_ != v2.nr_ || v1.ni_ != v2.ni_ || v1.rr_ != v2.rr_ || v1.ri_ != v2.ri_; }
  friend std::ostream& operator<<(std::ostream& os, QuBitField const& qubit_field);
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
