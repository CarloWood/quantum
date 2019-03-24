#include "sys.h"
#include "QuBitField.h"
#include "debug.h"
#include <iostream>
#include <sstream>

template<>
void print_formula_on(quantum::RationalsComplex const& number, std::ostream& os, bool& negate_first_term, bool print_negate_sign, bool is_factor)
{
  bool const have_real_part = number.m_real != 0;
  bool const have_imag_part = number.m_imag != 0;
  if (!have_real_part && !have_imag_part)
    os << '0';
  else
  {
    bool const has_multiple_terms = have_real_part && have_imag_part;
    bool const is_negative = (have_real_part && number.m_real < 0) || (!have_real_part && number.m_imag < 0);

    boost::multiprecision::mpq_rational real_part;
    boost::multiprecision::mpq_rational imaginary_part = number.m_imag;
    if (is_negative)
    {
      if (print_negate_sign)
        os << '-';
      real_part = -number.m_real;
      if (!have_real_part)
        imaginary_part = -number.m_imag;
    }
    else
      real_part = number.m_real;
    if (has_multiple_terms && is_factor)
    {
      os << '(';
      if (is_negative)
        imaginary_part = -number.m_imag;
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
    // negate_first_term should only be set when we're not printing a negate sign and we're negative (or zero).
    assert(!negate_first_term || (!print_negate_sign && is_negative));
    negate_first_term = false;
  }
}

template<>
void print_formula_on(quantum::QuBitField const& number, std::ostream& os, bool& negate_first_term, bool print_negate_sign, bool is_factor)
{
  if (number.is_zero())
    os << '0';
  else
  {
    bool const has_multiple_terms = number.has_multiple_terms();
    bool const is_negative = number.is_negative();
    bool const have_non_root_part = number.nr_ != 0 || number.ni_ != 0;
    bool const have_root_part = number.rr_ != 0 || number.ri_ != 0;

    // negate_first_term should only be set when we're not printing a negate sign and we're negative (or zero).
    assert(!negate_first_term || (!print_negate_sign && is_negative));

    int sign = 1;
    if (is_negative && print_negate_sign)
        os << '-';
    if (has_multiple_terms && is_factor)
    {
      os << '(';
      // Negate not just the first term but everything, if the number is negative.
      negate_first_term = false;
      sign = is_negative ? -1 : 1;
    }
    if (have_non_root_part)
    {
      quantum::RationalsComplex non_root(sign * number.nr_, sign * number.ni_);
      print_formula_on(non_root, os, negate_first_term, false, false);
    }
    if (have_root_part)
    {
      quantum::RationalsComplex root(sign * number.rr_, sign * number.ri_);
      if (have_non_root_part)
        os << (root.is_negative() ? " - " : " + ");
      if (root.is_unity())
        os << "√½";
      else
      {
        print_formula_on(root, os, negate_first_term, false, true);
        os << "·√½";
      }
    }
    if (has_multiple_terms && is_factor)
      os << ')';
    negate_first_term = false;
  }
}

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

std::ostream& operator<<(std::ostream& os, QuBitField const& number)
{
  bool negate_first_term = false;
  print_formula_on(number, os, negate_first_term, true, false);
  return os;
}

#if 0
std::string complex_to_string(mpq_rational r, mpq_rational i, bool need_parens)
{
  //DoutEntering(dc::notice, "complex_to_string(" << r << ", " << i << ", " << need_parens << ")");
  std::stringstream result;
  bool have_real_part = r != 0;
  bool have_imag_part = i != 0;
  bool imag_part_subtract = false;
  if (have_real_part)
  {
    if (need_parens && have_imag_part)
      result << '(';
    result << r;
    if (have_imag_part)
    {
      imag_part_subtract = i < 0;
      if (imag_part_subtract)
        result << " - ";
      else
        result << " + ";
    }
  }
  if (have_imag_part)
  {
    mpq_rational ip = imag_part_subtract ? -i : i;
    if (ip == 1)
      result << "i";
    else if (ip == -1)
      result << "-i";
    else
      result << ip << "\u00b7i"; // "·i"
    if (need_parens && have_real_part)
      result << ')';
  }
  return result.str();
}

std::string QuBitField::to_string(bool need_parens) const
{
  bool have_nr = nr_ != 0;
  bool have_ni = ni_ != 0;
  bool have_rr = rr_ != 0;
  bool have_ri = ri_ != 0;
  int n_parts = (have_nr ? 1 : 0) + (have_ni ? 1 : 0);
  int r_parts = (have_rr ? 1 : 0) + (have_ri ? 1 : 0);
  bool have_non_root_part = n_parts != 0;
  bool have_root_part = r_parts != 0;
  bool root_part_subtract = false;
  if (n_parts == 0 || (n_parts == 1 && !have_root_part))
    need_parens = false;
  std::string result = need_parens ? "(" : "";
  if (have_non_root_part)
  {
    result += complex_to_string(nr_, ni_, false);
    if (have_root_part)
    {
      root_part_subtract = (!have_rr && ri_ < 0) || (rr_ < 0 && !have_ri);
      if (root_part_subtract)
        result += " - ";
      else
        result += " + ";
    }
  }
  else if (!have_root_part)
    result = "0";
  if (have_root_part)
  {
    mpq_rational rrp = root_part_subtract ? -rr_ : rr_;
    mpq_rational rip = root_part_subtract ? -ri_ : ri_;
    if (rrp == 1 && rip == 0)
      result += "\u221a\u00bd"; // "√½"
    else if (rrp == -1 && rip == 0)
      result += "-\u221a\u00bd"; // "-√½"
    else
    {
      result += complex_to_string(rrp, rip, true);
      result += "\u00b7\u221a\u00bd"; // "·√½"
    }
  }
  if (need_parens)
    result += ')';
  return result;
}

std::ostream& operator<<(std::ostream& os, QuBitField const& qubit_field)
{
  os << qubit_field.to_string();
  return os;
}

std::ostream& operator<<(std::ostream& os, QuBitField::Element const& element)
{
  //DoutEntering(dc::notice, "operator<<(Element:{{nr_:" << element.m_qubit_field_ptr->nr_ << ", ni_:" << element.m_qubit_field_ptr->ni_ << ", rr_:" << element.m_qubit_field_ptr->rr_ << ", ri_:" << element.m_qubit_field_ptr->ri_ << "}, " << element.m_what << "})");
  // This may assume that what is printed is always non-zero.
  assert(element.m_what == -1 || element.m_what == -2 || element.m_what == -3 || element.m_what == 1 || element.m_what == 2 || element.m_what == 3);
  int sign = element.m_what < 0 ? -1 : 1;
  if (element.m_what == -1 || element.m_what == 1)
    os << complex_to_string(sign * element.m_qubit_field_ptr->nr_, 0, false);
  else if (element.m_what == -2 || element.m_what == 2)
    os << complex_to_string(0, sign * element.m_qubit_field_ptr->ni_, false);
  else if ((element.m_qubit_field_ptr->rr_ == 1 || element.m_qubit_field_ptr->rr_ == -1) && element.m_qubit_field_ptr->ri_ == 0)
  {
    if (element.m_qubit_field_ptr->rr_ != sign)
      os << '-';
    os << "\u221a\u00bd"; // "√½"
  }
  else
    os << complex_to_string(sign * element.m_qubit_field_ptr->rr_, sign * element.m_qubit_field_ptr->ri_, true) << "\u00b7\u221a\u00bd"; // "·√½"
  return os;
}
#endif

} // namespace quantum
