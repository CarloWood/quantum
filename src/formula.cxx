#include "sys.h"
#include "formula.h"
#include <iostream>

namespace formula {

std::ostream& operator<<(std::ostream& os, Sum const& number)
{
  if (number.starts_with_a_minus())
    os << '-';
  number.print_on(os, false, false);
  return os;
}

std::ostream& operator<<(std::ostream& os, Product const& number)
{
  if (number.starts_with_a_minus())
    os << '-';
  number.print_on(os, false, false);
  return os;
}

} // namespace formula
