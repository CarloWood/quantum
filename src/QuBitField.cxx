#include "sys.h"
#include "QuBitField.h"
#include "debug.h"
#include <iostream>
#include <sstream>

namespace formula {

// Specialization for quantum::RationalsComplex.

template<>
void print_formula_on(quantum::RationalsComplex const& number, std::ostream& os, bool negate_all_terms, bool is_factor)
{
  bool const have_real_part = number.m_real != 0;
  bool const have_imag_part = number.m_imag != 0;
  if (!have_real_part && !have_imag_part)
    os << '0';
  else
  {
    boost::multiprecision::mpq_rational real_part = negate_all_terms ? -number.m_real : number.m_real;
    boost::multiprecision::mpq_rational imaginary_part = negate_all_terms ? -number.m_imag : number.m_imag;

    bool const has_multiple_terms = have_real_part && have_imag_part;
    bool const starts_with_a_minus = (have_real_part && real_part < 0) || (!have_real_part && imaginary_part < 0);

    if (starts_with_a_minus)
    {
      if (have_real_part)
        real_part = -real_part;
      else
        imaginary_part = -imaginary_part;
    }
    if (has_multiple_terms && is_factor)
    {
      os << '(';
      if (starts_with_a_minus)
        imaginary_part = -imaginary_part;
    }
    if (have_real_part)
      os << real_part;
    if (imaginary_part > 0)
    {
      if (have_real_part)
        os << " + ";
      if (imaginary_part == 1)
        os << "i";
      else
        os << imaginary_part << "\u00b7i"; // "·i"
    }
    else if (imaginary_part < 0)
    {
      if (have_real_part)
        os << " - ";
      else
        os << '-';
      if (imaginary_part == -1)
        os << "i";
      else
        os << -imaginary_part << "\u00b7i"; // "·i"
    }
    if (has_multiple_terms && is_factor)
      os << ')';
  }
}

} // namespace formula

namespace quantum {

QuBitField operator*(QuBitField const& v1, QuBitField const& v2)
{
  QuBitField result;
  mpq_rational half(1, 2);
  result.nr_ = v1.nr_ * v2.nr_ - v1.ni_ * v2.ni_ + half * (v1.rr_ * v2.rr_ - v1.ri_ * v2.ri_);
  result.ni_ = v1.nr_ * v2.ni_ + v1.ni_ * v2.nr_ + half * (v1.rr_ * v2.ri_ + v1.ri_ * v2.rr_);
  result.rr_ = v1.rr_ * v2.nr_ - v1.ri_ * v2.ni_ + v1.nr_ * v2.rr_ - v1.ni_ * v2.ri_;
  result.ri_ = v1.rr_ * v2.ni_ + v1.ri_ * v2.nr_ + v1.nr_ * v2.ri_ + v1.ni_ * v2.rr_;
  return result;
}

void QuBitField::print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const
{
  if (is_zero())
    os << '0';
  else
  {
    bool const has_multiple_terms = this->has_multiple_terms();
    bool const starts_with_a_minus = this->starts_with_a_minus() != negate_all_terms;
    bool const have_non_root_part = nr_ != 0 || ni_ != 0;
    bool const have_root_part = rr_ != 0 || ri_ != 0;

    if (has_multiple_terms && is_factor)
    {
      os << '(';
      // Negate not just the first term but everything, if the number is negative.
      if (starts_with_a_minus)
        negate_all_terms = !negate_all_terms;
    }
    if (have_non_root_part)
    {
      quantum::RationalsComplex non_root(nr_, ni_);
      formula::print_formula_on(non_root, os, negate_all_terms, false);
    }
    if (have_root_part)
    {
      quantum::RationalsComplex root(rr_, ri_);
      if (have_non_root_part)
        os << ((root.starts_with_a_minus() != negate_all_terms) ? " - " : " + ");
      if (root.is_unity())
        os << "√½";
      else
      {
        formula::print_formula_on(root, os, negate_all_terms, true);
        os << "·√½";
      }
    }
    if (has_multiple_terms && is_factor)
      os << ')';
  }
}

} // namespace quantum
