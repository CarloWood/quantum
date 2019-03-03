#include "sys.h"
#include "Complex.h"
#include "utils/almost_equal.h"

namespace quantum {

// Return true when z1 and z2 are (almost) equal.
bool operator==(Complex const& z1, Complex const& z2)
{
  return utils::almost_equal(z1.m_value, z2.m_value, M_PI / 1024);
}

std::ostream& operator<<(std::ostream& os, Complex const& z)
{
  bool has_real_part = !utils::almost_equal(z.m_value, -std::conj(z.m_value), M_PI / 1024);
  bool has_imaginary_part = !utils::almost_equal(z.m_value, std::conj(z.m_value), M_PI / 1024);
  if (has_real_part && has_imaginary_part)
    os << '(';
  if (has_real_part || !has_imaginary_part)
  {
    os << z.m_value.real();
    if (has_imaginary_part)
    {
      auto im = z.m_value.imag();
      os << (im < 0 ? " - " : " + ") << std::abs(im) << " i";
    }
  }
  else
    os << z.m_value.imag() << " i";
  if (has_real_part && has_imaginary_part)
    os << ')';
  return os;
}

} // namespace quantum
