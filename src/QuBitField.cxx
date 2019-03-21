#include "sys.h"
#include "QuBitField.h"
#include "debug.h"
#include <iostream>
#include <sstream>

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

std::string complex_to_string(mpq_rational r, mpq_rational i, bool need_parens)
{
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

} // namespace quantum
