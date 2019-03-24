#pragma once

#include "debug.h"
#include <iostream>
#include <sstream>

template<typename T>
void print_formula_on(T const& number, std::ostream& os, bool& negate_first_term, bool print_negate_sign = true, bool UNUSED_ARG(is_factor) = false)
{
  // The general type T must be like an int: usable as both term or factor without parentheses,
  // and when printed to an ostream, start with a '-' character when negative while when that
  // minus sign is removed from the printed string that is the same as negating the number.

  // If negate_first_term is set then that is in order to get rid of a minus sign of the first
  // non-zero term (the number is either negative or zero). If we wanted to print that minus
  // sign, we wouldn't be asking to remove it.
  //
  // So, the logic is:
  //
  // A non-negative number can have negate_first_term false and print_negate_sign true or false,
  // while a negative number can have negate_first_term false and print_negate_sign true, or
  // negate_first_term true and print_negate_sign false.
  //
  //   negate_first_term:  0    0    1    1
  //   print_negate_sign:  0    1    0    1
  //      sign of number: >=0  any  <=0   -
  //
  ASSERT(!(print_negate_sign && negate_first_term));

  if (print_negate_sign)
    os << number;                       // negate_first_term is false.
  else
  {
    std::stringstream ss;
    ss << number;
    assert(!ss.str().empty());
    // A number may never start with a '('. Either use Sum or specialize this template function.
    assert(ss.str()[0] != '(');
    int offset = ss.str()[0] == '-' ? 1 : 0;
    os << ss.str().substr(offset);
    if (offset == 1)
      negate_first_term = false;        // The first negative term was found and negated.
  }
}

struct Sum
{
  virtual bool has_multiple_terms() const = 0;
  virtual bool is_zero() const = 0;
};
