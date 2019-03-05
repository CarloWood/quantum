#include "sys.h"
#include "Complex.h"
#include "utils/almost_equal.h"
#include "debug.h"
#include <array>
#include <iomanip>

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

void Complex::snap(double abs_relative_error)
{
  DoutEntering(dc::notice|flush_cf, "Complex::snap(" << abs_relative_error << ") [with m_value = " << m_value << "]");
  static std::array<std::pair<double, char const*>, 33> well_known_values = {{
    // Let x = 1/sqrt(2)
    { 1,                          "1" },
    { 0.707106781186547524400844, "x" },
    { 0.5,                        "x^2" },
    { 0.353553390593273762200422, "x^3" },
    { 0.25,                       "x^4" },
    { 0.176776695296636881100211, "x^5" },
    { 0.125,                      "x^6" },
    { 0.088388347648318440550106, "x^7" },
    { 0.0625,                     "x^8" },
    { 0.044194173824159220275053, "x^9" },
    { 0.03125,                    "x^10" },

    { 0.853553390593273762200422, "x^2 + x^3" },
    { 0.146446609406726237799578, "x^2 - x^3" },
    { 0.603553390593273762200422, "x^3 + x^4" },
    { 0.103553390593273762200422, "x^3 - x^4" },
    { 0.457106781186547524400844, "x - x^4" },
    { 0.75,                       "1 - x^4" },
    { 0.957106781186547524400844, "x + x^4" },
    { 0.323223304703363118899789, "x^2 - x^5" },
    { 0.073223304703363118899789, "x^4 - x^5" },
    { 0.280330085889910643300633, "x^3 - x^4 + x^5" },
    { 0.573223304703363118899789, "x^2 + x^4 - x^5" },
    { 0.426776695296636881100211, "x^4 + x^5" },
    { 0.676776695296636881100211, "x^2 + x^5" },
    { 0.883883476483184405501055, "x + x^5" },
    { 0.926776695296636881100211, "x^2 + x^4 + x^5" },
    { 0.780330085889910643300633, "x + x^4 - x^5" },
    { 0.530330085889910643300633, "x - x^5" },
    { 0.551776695296636881100211, "x^4 + x^5 + x^6" },
    { 0.875,                      "1 - x^6" },
    { 0.405330085889910643300633, "x^3 - x^4 + x^5 + x^6" },
    { 0.375,                      "x^4 + x^6" },
    { 0.625,                      "x^2 + x^6" },
  }};
  double real = m_value.real();
  double imag = m_value.imag();
  double mag = std::abs(m_value);
  if (mag < 1e-10)
  {
    m_value = 0;
    return;
  }
  for (int real_or_imag = 0; real_or_imag < 2; ++real_or_imag)
  {
    double& component{(real_or_imag == 0) ? real : imag};
    // Test for zero.
    if (std::abs(component) < abs_relative_error * mag)
    {
      component = 0.0;
      continue;
    }
    int const sign = component < 0 ? -1 : 1;
    double abs_component = std::abs(component);
    // Test for other values.
    bool found = false;
    for (unsigned int well_known_value = 0; well_known_value < well_known_values.size(); ++well_known_value)
    {
      double const reference = well_known_values[well_known_value].first;
      if (utils::almost_equal(abs_component, reference, abs_relative_error))
      {
        component = sign * reference;
        found = true;
        break;
      }
    }
    if (!found)
    {
      Dout(dc::warning, "Could not find well-known value for x = " << std::setprecision(12) << abs_component);
      // Try hard to construct it from well-known values.
      for (unsigned int wkv1 = 0; wkv1 < well_known_values.size(); ++wkv1)
      {
        for (unsigned int wkv2 = wkv1 + 1; wkv2 < well_known_values.size(); ++wkv2)
        {
          double sum = well_known_values[wkv1].first + well_known_values[wkv2].first;
          double diff = well_known_values[wkv1].first - well_known_values[wkv2].first;
          if (utils::almost_equal(sum, abs_component, 1e-8))
            Dout(dc::warning, "Found it to be equal to " << well_known_values[wkv1].second << " + " << well_known_values[wkv2].second);
          if (utils::almost_equal(diff, abs_component, 1e-8))
            Dout(dc::warning, "Found it to be equal to " << well_known_values[wkv1].second << " - (" << well_known_values[wkv2].second << ')');
        }
      }
    }
  }
  m_value = std::complex<double>{real, imag};
}

} // namespace quantum
