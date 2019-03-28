#pragma once

#include "debug.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace formula {

struct Sum
{
  virtual ~Sum() { }
  virtual bool starts_with_a_minus() const = 0;         // The derived class must call sum_starts_with_a_minus.
  virtual bool has_multiple_terms() const = 0;          // The derived class must call sum_has_multiple_terms.
  virtual bool is_zero() const = 0;                     // The derived class must call sum_is_zero.
  virtual bool is_unity() const = 0;                    // The derived class must call sum_is_unity.
  virtual void print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const = 0; // The derived class must call print_sum_on.

  friend std::ostream& operator<<(std::ostream& os, Sum const& number);

 protected:
  template<class InputIt>
  void print_sum_on(InputIt first, InputIt last, std::ostream& os, bool negate_all_terms, bool is_factor) const;

  // Return true iff the first terms starts with a minus sign.
  template<class SumContainer>
  bool sum_starts_with_a_minus(SumContainer const& terms) const;

  // Count the number of non-zero terms and return true iff there is more than one.
  template<class SumContainer>
  bool sum_has_multiple_terms(SumContainer const& terms) const;

  // Returns true if the sum has no terms or only zero terms.
  template<class SumContainer>
  bool sum_is_zero(SumContainer const& terms) const;

  // Returns true if the sum has exactly one term, which is unity (1 or -1).
  template<class SumContainer>
  bool sum_is_unity(SumContainer const& terms) const;
};

struct Product
{
  virtual ~Product() { }
  virtual bool starts_with_a_minus() const = 0;                 // The derived class must call product_starts_with_a_minus.
  virtual bool is_zero() const = 0;                             // The derived class must call product_is_zero.
  virtual bool is_unity() const = 0;                            // The derived class must call product_is_unity.
  virtual void print_on(std::ostream& os, bool negate_all_terms, bool) const = 0; // The derived class must call print_product_on.
  bool has_multiple_terms() const { return false; }

  // Default multiplication operator.
  virtual char const* multiply_string() const { return " * "; }

  friend std::ostream& operator<<(std::ostream& os, Product const& number);

 protected:
  template<class InputIt>
  void print_product_on(InputIt first, InputIt last, std::ostream& os) const;

  template<class ProductContainer>
  bool product_starts_with_a_minus(ProductContainer const& factors) const;

  template<class ProductContainer>
  bool product_is_zero(ProductContainer const& factors) const;

  template<class ProductContainer>
  bool product_is_unity(ProductContainer const& factors) const;
};

template<typename T>
void print_formula_on(T const& number, std::ostream& os, bool negate_all_terms, bool is_factor = false)
{
  if constexpr (!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value)
  {
    // The general type T must be like an int: usable as both term or factor without parentheses,
    // and when printed to an ostream starts with a '-' character when negative while when that
    // minus sign is removed from the printed string that is the same as negating the number.

    std::stringstream ss;
    ss << number;
    assert(!ss.str().empty());
    // A number may never start with a '('. Either use Sum or specialize this template function.
    assert(ss.str()[0] != '(');
    // Always remove any leading minus sign.
    int offset = ss.str()[0] == '-' ? 1 : 0;
    os << ss.str().substr(offset);
  }
  else
    number.print_on(os, negate_all_terms, is_factor);
}

template<typename T, typename std::enable_if<!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value, int>::type = 0>
bool is_zero(T const& number)
{
  return number == 0;
}

inline bool is_zero(Sum const& number)
{
  return number.is_zero();
}

inline bool is_zero(Product const& number)
{
  return number.is_zero();
}

template<typename T, typename std::enable_if<!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value, int>::type = 0>
bool is_unity(T const& number)
{
  return number == 1 || number == -1;
}

inline bool is_unity(Sum const& number)
{
  return number.is_unity();
}

inline bool is_unity(Product const& number)
{
  return number.is_unity();
}

template<typename T, typename std::enable_if<!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value, int>::type = 0>
bool starts_with_a_minus(T const& number)
{
  return number < 0;
}

inline bool starts_with_a_minus(Sum const& number)
{
  return number.starts_with_a_minus();
}

inline bool starts_with_a_minus(Product const& number)
{
  return number.starts_with_a_minus();
}

template<class InputIt>
void Sum::print_sum_on(InputIt first, InputIt last, std::ostream& os, bool negate_all_terms, bool is_factor) const
{
  if (is_zero())
  {
    os << "0";
    return;
  }
  bool const starts_with_a_minus_ = starts_with_a_minus() != negate_all_terms;
  bool const has_multiple_terms_ = has_multiple_terms();
  bool const needs_parens = has_multiple_terms_ && is_factor;
  bool const toggle_sign_all_terms = (needs_parens && starts_with_a_minus_) != negate_all_terms;
  if (needs_parens)
    os << '(';
  bool first_term = true;
  do
  {
    if (!formula::is_zero(*first))
    {
      if (!first_term)
        os << (formula::starts_with_a_minus(*first) != toggle_sign_all_terms ? " - " : " + ");
      print_formula_on(*first, os, toggle_sign_all_terms, false);
      first_term = false;
    }
  }
  while (++first != last);
  if (needs_parens)
    os << ')';
}

template<class SumContainer>
bool Sum::sum_starts_with_a_minus(SumContainer const& terms) const
{
  for (auto&& term : terms)
  {
    if (term.is_zero())
      continue;
    return term.starts_with_a_minus();
  }
  return false;
}

template<class SumContainer>
bool Sum::sum_has_multiple_terms(SumContainer const& terms) const
{
  int cnt = 0;
  for (auto&& term : terms)
  {
    if (term.is_zero())
      continue;
    if (++cnt > 1)
      return true;
  }
  return false;
}

template<class SumContainer>
bool Sum::sum_is_zero(SumContainer const& terms) const
{
  return !std::any_of(terms.begin(), terms.end(), [](typename SumContainer::value_type const& i){ return !i.is_zero(); });
}

template<class SumContainer>
bool Sum::sum_is_unity(SumContainer const& terms) const
{
  return terms.size() == 1 && formula::is_unity(terms[0]);
}

template<class InputIt>
void Product::print_product_on(InputIt first, InputIt last, std::ostream& os) const
{
  if (is_zero())
  {
    os << "0";
    return;
  }
  if (is_unity())
  {
    os << "1";
    return;
  }
  bool first_term = true;
  do
  {
    if (!formula::is_unity(*first))
    {
      if (!first_term)
        os << multiply_string();
      print_formula_on(*first, os, false, true);
      first_term = false;
    }
  }
  while (++first != last);
}

template<class ProductContainer>
bool Product::product_starts_with_a_minus(ProductContainer const& factors) const
{
  bool result = false;
  std::for_each(factors.begin(), factors.end(), [&result](typename ProductContainer::value_type const& factor){ if (factor.starts_with_a_minus()) result = !result; });
  return result;
}

template<class ProductContainer>
bool Product::product_is_zero(ProductContainer const& factors) const
{
  return std::any_of(factors.begin(), factors.end(), [](typename ProductContainer::value_type const& factor){ return factor.is_zero(); });
}

template<class ProductContainer>
bool Product::product_is_unity(ProductContainer const& factors) const
{
  return std::all_of(factors.begin(), factors.end(), [](typename ProductContainer::value_type const& factor){ return factor.is_unity(); });
}

} // namespace formula
