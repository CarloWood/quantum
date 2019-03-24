#include "sys.h"
#include "debug.h"
#include "QuBitField.h"
#include <sstream>
#include <complex>

extern char const* ft_expected_output[];

struct Factor
{
};

template<typename T>
struct Sum2 : public Factor
{
  std::vector<T> m_sum;

  Sum2() { }
  Sum2(std::initializer_list<T> const& il) : m_sum(il) { }

  // Default addition operator.
  virtual char const* add_string() const { return " + "; }
  // Default subtraction operator.
  virtual char const* subtract_string() const { return " - "; }
  // Default negation operator.
  virtual char const* negate_string() const { return "-"; }

  // Print the sum to os.
  //
  // Sum          print_negate_sign/is_factor (default put between square brackets)
  //           [true/false]    [true]/true      false/[false]   false/true
  //  0        "0"             "0"             "0"             "0"
  //  42       "42"            "42"            "42"            "42"
  // -42       "-42"           "-42"           "42"*           "42"*
  //  42 + 7   "42 + 7"        "(42 + 7)"      "42 + 7"        "(42 + 7)"
  //  42 - 7   "42 - 7"        "(42 - 7)"      "42 - 7"        "(42 - 7)"
  // -42 + 7   "-42 + 7"       "-(42 - 7)"     "42 + 7"*       "(42 - 7)"*
  // -42 - 7   "-42 - 7"       "-(42 + 7)"     "42 - 7"*       "(42 + 7)"*
  //
  // *) negate_first_term is set to false.
  void print_on(std::ostream& os, bool& negate_first_term, bool print_negate_sign = true, bool is_factor = false) const
  {
    os << "FIXME";
  }

  friend std::ostream& operator<<(std::ostream& os, Sum2 const& sum) { bool nns; sum.print_on(os, nns); return os; }
};

template<typename T>
struct Product2
{
  // Default multiplication operator.
  virtual char const* multiplication_string() const { return " * "; }
};

//============================================================================
// Test program begins here.
//

struct ASum : public Sum2<int>
{
  using Sum2<int>::Sum2;
};

struct AProduct : public Product2<ASum>
{
  using Product2::Product2;
};

struct ASum2 : public Sum2<AProduct>
{
  using Sum2<AProduct>::Sum2;
};

int main()
{
  Debug(NAMESPACE_DEBUG::init());

  // Test print_formula_on for quantum::RationalsComplex.
  {
    Dout(dc::notice, "TEST OF print_formula_on(quantum::RationalsComplex const&, ...).");
    debug::Indent indent(2);

    char const* expected[50] = {
      "-2 - 2·i",
      "-(2 + 2·i)",
      "-2 - i",
      "-(2 + i)",
      "-2",
      "-2",
      "-2 + i",
      "-(2 - i)",
      "-2 + 2·i",
      "-(2 - 2·i)",
      "-1 - 2·i",
      "-(1 + 2·i)",
      "-1 - i",
      "-(1 + i)",
      "-1",
      "-1",
      "-1 + i",
      "-(1 - i)",
      "-1 + 2·i",
      "-(1 - 2·i)",
      "-2·i",
      "-2·i",
      "-i",
      "-i",
      "0",
      "0",
      "i",
      "i",
      "2·i",
      "2·i",
      "1 - 2·i",
      "(1 - 2·i)",
      "1 - i",
      "(1 - i)",
      "1",
      "1",
      "1 + i",
      "(1 + i)",
      "1 + 2·i",
      "(1 + 2·i)",
      "2 - 2·i",
      "(2 - 2·i)",
      "2 - i",
      "(2 - i)",
      "2",
      "2",
      "2 + i",
      "(2 + i)",
      "2 + 2·i",
      "(2 + 2·i)",
    };
    int test_cnt = 0;
    for (int k = -2; k <= 2; ++k)
      for (int l = -2; l <= 2; ++l)
      {
        quantum::RationalsComplex v(k, l);
        for (int t1 = 0; t1 < 2; ++t1)
        {
          bool const is_factor = t1;
          bool const is_negative = v.m_real < 0 || (v.m_real == 0 && v.m_imag < 0);

          Dout(dc::notice|continued_cf|flush_cf, "Input: \"" << v.m_real << " + " << v.m_imag << " i\", is_factor = " << is_factor << "; output: \"");

          bool negate_first_term0 = is_negative;
          std::stringstream ss_no_print_negate_sign;
          print_formula_on(v, ss_no_print_negate_sign, negate_first_term0, false, is_factor);
          auto si = ss_no_print_negate_sign.str().find_first_not_of('(');
          std::string output0 = (is_negative ? "-" : "") + ss_no_print_negate_sign.str();
          Dout(dc::finish, output0 << "\".");
          // Never starts with a minus sign.
          ASSERT(ss_no_print_negate_sign.str()[si] != '-');
          // Only set for negative values, it should always be reset.
          ASSERT(!negate_first_term0);

          std::stringstream ss_print_negate_sign;
          bool negate_first_term1 = false;
          print_formula_on(v, ss_print_negate_sign, negate_first_term1, true, is_factor);
          // Should always be equal when minus sign is prepended.
          ASSERT(ss_print_negate_sign.str() == output0);

          ASSERT(output0 == expected[test_cnt]);
          ++test_cnt;
        }
      }
    Dout(dc::notice, "SUCCESS.");
  }

  // Test print_formula_on for quantum::QuBitField.
  {
    Dout(dc::notice, "TEST OF print_formula_on(quantum::QuBitField const&, ...).");
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
              bool const is_negative = v.is_negative();

              Dout(dc::notice|continued_cf|flush_cf, "Input: \"" << k << " + " << l << " i + (" << m << " + " << n << " i)·√½" << "\", is_factor = " << is_factor << "; output: \"");

              bool negate_first_term0 = is_negative;
              std::stringstream ss_no_print_negate_sign;
              print_formula_on(v, ss_no_print_negate_sign, negate_first_term0, false, is_factor);
              auto si = ss_no_print_negate_sign.str().find_first_not_of('(');
              std::string output0 = (is_negative ? "-" : "") + ss_no_print_negate_sign.str();
              Dout(dc::finish, output0 << "\".");
              // Never starts with a minus sign.
              ASSERT(ss_no_print_negate_sign.str()[si] != '-');
              // Only set for negative values, it should always be reset.
              ASSERT(!negate_first_term0);

              std::stringstream ss_print_negate_sign;
              bool negate_first_term1 = false;
              print_formula_on(v, ss_print_negate_sign, negate_first_term1, true, is_factor);
              // Should always be equal when minus sign is prepended.
              ASSERT(ss_print_negate_sign.str() == output0);

              ASSERT(output0 == ft_expected_output[test_cnt]);
              ++test_cnt;
            }
          }
    Dout(dc::notice, "SUCCESS.");
  }

#if 1
  //ASum foo{42, 1};
#else
  AProduct p1{ASum{-42, -1, -2, -3}, ASum{4, 5, 6}};
  AProduct p2{ASum{-42, 1, 2, 3}, ASum{7, 8, 9}};

  ASum2 bar{p1, p2};
#endif

  //Dout(dc::notice, foo);
}
