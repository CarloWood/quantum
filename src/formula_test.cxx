#include "sys.h"
#include "debug.h"
#include "QuBitField.h"
#include <sstream>
#include <complex>

extern char const* ft_expected_output[];
extern char const* ft_expected_output_sum[];

struct MySum : public formula::Sum<std::vector<quantum::QuBitField>>
{
  using formula::Sum<std::vector<quantum::QuBitField>>::Sum;
};

//============================================================================
// Test program begins here.
//

struct MyProduct : public formula::Product<std::vector<MySum>>
{
  using formula::Product<std::vector<MySum>>::Product;
};

struct MySum2 : public formula::Sum<std::vector<MyProduct>>
{
  using formula::Sum<std::vector<MyProduct>>::Sum;
};

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  // Test print_formula_on for quantum::RationalsComplex.
  {
    Dout(dc::notice, "TEST OF print_formula_on(quantum::RationalsComplex const&, ...).");
    debug::Indent indent(2);

    char const* expected[50] = {
      "-2 - 2\u00b7i",
      "-(2 + 2\u00b7i)",
      "-2 - i",
      "-(2 + i)",
      "-2",
      "-2",
      "-2 + i",
      "-(2 - i)",
      "-2 + 2\u00b7i",
      "-(2 - 2\u00b7i)",
      "-1 - 2\u00b7i",
      "-(1 + 2\u00b7i)",
      "-1 - i",
      "-(1 + i)",
      "-1",
      "-1",
      "-1 + i",
      "-(1 - i)",
      "-1 + 2\u00b7i",
      "-(1 - 2\u00b7i)",
      "-2\u00b7i",
      "-2\u00b7i",
      "-i",
      "-i",
      "0",
      "0",
      "i",
      "i",
      "2\u00b7i",
      "2\u00b7i",
      "1 - 2\u00b7i",
      "(1 - 2\u00b7i)",
      "1 - i",
      "(1 - i)",
      "1",
      "1",
      "1 + i",
      "(1 + i)",
      "1 + 2\u00b7i",
      "(1 + 2\u00b7i)",
      "2 - 2\u00b7i",
      "(2 - 2\u00b7i)",
      "2 - i",
      "(2 - i)",
      "2",
      "2",
      "2 + i",
      "(2 + i)",
      "2 + 2\u00b7i",
      "(2 + 2\u00b7i)",
    };
    int test_cnt = 0;
    for (int k = -2; k <= 2; ++k)
      for (int l = -2; l <= 2; ++l)
      {
        quantum::RationalsComplex v(k, l);
        for (int t1 = 0; t1 < 2; ++t1)
        {
          bool const is_factor = t1;
          bool const starts_with_a_minus = v.m_real < 0 || (v.m_real == 0 && v.m_imag < 0);

          Dout(dc::notice|continued_cf|flush_cf, "Input: \"" << v.m_real << " + " << v.m_imag << " i\", is_factor = " << is_factor << "; output: \"");

          std::stringstream ss;
          formula::print_formula_on(v, ss, false, is_factor);
          auto si = ss.str().find_first_not_of('(');
          std::string output0 = (starts_with_a_minus ? "-" : "") + ss.str();
          Dout(dc::finish, output0 << "\".");
          // Never starts with a minus sign.
          ASSERT(ss.str()[si] != '-');

          ASSERT(output0 == expected[test_cnt]);
          ++test_cnt;
        }
      }
    Dout(dc::notice, "SUCCESS.");
  }

  // Test print_on for quantum::QuBitField.
  {
    Dout(dc::notice, "TEST OF quantum::QuBitField::print_on(...).");
    debug::Indent indent(2);

    int test_cnt = 0;
    for (int k = -2; k <= 2; ++k)
      for (int l = -2; l <= 2; ++l)
        for (int m = -2; m <= 2; ++m)
          for (int n = -2; n <= 2; ++n)
          {
            quantum::QuBitField v(k, l, m, n);
            for (int t1 = 0; t1 < 2; ++t1)
            {
              bool const is_factor = t1;
              bool const starts_with_a_minus = v.starts_with_a_minus();

              Dout(dc::notice|continued_cf|flush_cf, "Input: \"" << k << " + " << l << " i + (" << m << " + " << n << " i)\u00b7\u221a\u00bd" << "\", is_factor = " << is_factor << "; output: \"");

              std::stringstream ss;
              v.print_on(ss, false, is_factor);
              auto si = ss.str().find_first_not_of('(');
              std::string output0 = (starts_with_a_minus ? "-" : "") + ss.str();
              Dout(dc::finish, output0 << "\".");
              // Never starts with a minus sign.
              ASSERT(ss.str()[si] != '-');

              ASSERT(output0 == ft_expected_output[test_cnt]);
              ++test_cnt;
            }
          }
    Dout(dc::notice, "SUCCESS.");
  }

  // Test print_on for formula::Sum
  {
    Dout(dc::notice, "TEST OF Sum::print_on(...).");
    debug::Indent indent(2);

    int test_cnt = 0;
    for (int k = -1; k <= 1; ++k)
      for (int l = -1; l <= 1; ++l)
        for (int m = -1; m <= 1; ++m)
        {
          quantum::QuBitField v(k, l, m, -1);
          MySum my_sum{{v, v}};
          for (int t1 = 0; t1 < 2; ++t1)
          {
            bool const is_factor = t1;

            Dout(dc::notice|continued_cf|flush_cf, "Input: \"" << k << " + " << l << " i + (" << m << " - i)\u00b7\u221a\u00bd" << "\", is_factor = " << is_factor << "; output: \"");
            std::stringstream ss;
            if (my_sum.starts_with_a_minus())
              ss << '-';
            my_sum.print_on(ss, false, is_factor);
            Dout(dc::finish, ss.str() << "\".");

            ASSERT(ss.str() == ft_expected_output_sum[test_cnt]);
            ++test_cnt;
          }
        }
    Dout(dc::notice, "SUCCESS.");
  }

  MyProduct p1{{MySum{{-42, -1, -2, -3}}, MySum{{4, 5, 6}}}};
  MyProduct p2{{MySum{{-42, 1, 2, 3}}, MySum{{7, 8, 9}}}};

  MySum2 bar{{p1, p2}};

  Dout(dc::notice, bar);
}
