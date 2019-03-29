#pragma once

#include "debug.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace formula {

template<class SumContainer>
struct Sum
{
  using formula_tag = void;
  using container_type = SumContainer;

 protected:
  SumContainer m_sum;

 public:
  Sum(SumContainer&& sum) : m_sum(std::move(sum)) { }
  Sum(SumContainer const& sum) : m_sum(sum) { }
  virtual ~Sum() { }

  virtual bool starts_with_a_minus() const;
  virtual bool has_multiple_terms() const;
  virtual bool is_zero() const;
  virtual bool is_unity() const;
  virtual void print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const;
};

template<class ProductContainer>
struct Product
{
  using formula_tag = void;
  using container_type = ProductContainer;

 protected:
  ProductContainer m_product;

 public:
  Product(ProductContainer&& product) : m_product(std::move(product)) { }
  Product(ProductContainer const& product) : m_product(product) { }
  virtual ~Product() { }

  virtual bool starts_with_a_minus() const;
  virtual bool is_zero() const;
  virtual bool is_unity() const;
  virtual void print_on(std::ostream& os) const;

  // Default multiplication operator.
  virtual char const* multiply_string() const { return " * "; }
};

template<typename T, typename = void>
struct is_formula : std::false_type
{
};

template<typename T>
struct is_formula<T, typename T::formula_tag> : std::true_type
{
};

//----------------------------------------------------------------------------
// print_formula_on
//

template<typename T, typename std::enable_if<!is_formula<T>::value, int>::type = 0>
void print_formula_on(T const& number, std::ostream& os, bool UNUSED_ARG(negate_all_terms), bool UNUSED_ARG(is_factor) = false)
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

template<typename SumContainer>
inline void print_formula_on(Sum<SumContainer> const& number, std::ostream& os, bool negate_all_terms, bool is_factor = false)
{
  number.print_on(os, negate_all_terms, is_factor);
}

template<typename ProductContainer>
inline void print_formula_on(Product<ProductContainer> const& number, std::ostream& os, bool UNUSED_ARG(negate_all_terms), bool UNUSED_ARG(is_factor) = false)
{
  number.print_on(os);
}

//----------------------------------------------------------------------------
// is_zero
//

template<typename T, typename std::enable_if<!is_formula<T>::value, int>::type = 0>
inline bool is_zero(T const& number)
{
  return number == 0;
}

template<typename T, typename std::enable_if<is_formula<T>::value, int>::type = 0>
inline bool is_zero(T const& number)
{
  return number.is_zero();
}

//----------------------------------------------------------------------------
// is_unity
//

template<typename T, typename std::enable_if<!is_formula<T>::value, int>::type = 0>
inline bool is_unity(T const& number)
{
  return number == 1 || number == -1;
}

template<typename T, typename std::enable_if<is_formula<T>::value, int>::type = 0>
inline bool is_unity(T const& number)
{
  return number.is_unity();
}

//----------------------------------------------------------------------------
// starts_with_a_minus
//

template<typename T, typename std::enable_if<!is_formula<T>::value, int>::type = 0>
inline bool starts_with_a_minus(T const& number)
{
  return number < 0;
}

template<typename T, typename std::enable_if<is_formula<T>::value, int>::type = 0>
inline bool starts_with_a_minus(T const& number)
{
  return number.starts_with_a_minus();
}

//----------------------------------------------------------------------------
// Definitions of Sum member functions.
//

// If these member functions are overridden in the derived class,
// then the following implementations might not even compile.
// Therefore test for the existance of SumContainer::const_iterator
// in all cases.

template<typename T, typename = void>
struct has_const_iterator : std::false_type {};

template<typename T>
struct has_const_iterator<T, std::void_t<decltype(cbegin(std::declval<T>()))>> : std::true_type {};

template<class SumContainer>
void Sum<SumContainer>::print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const
{
  if constexpr (has_const_iterator<SumContainer>::value)
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
    auto first = m_sum.begin();
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
    while (++first != m_sum.end());
    if (needs_parens)
      os << ')';
  }
  else
  {
    // "Derived class needs to override print_on() const.
    assert(false);
  }
}

template<class SumContainer>
bool Sum<SumContainer>::starts_with_a_minus() const
{
  if constexpr (has_const_iterator<SumContainer>::value)
  {
    for (auto&& term : m_sum)
      if (!formula::is_zero(term))
        return formula::starts_with_a_minus(term);
  }
  else
  {
    // Derived class needs to override starts_with_a_minus() const.
    assert(false);
  }
  return false;
}

template<class SumContainer>
bool Sum<SumContainer>::has_multiple_terms() const
{
  if constexpr (has_const_iterator<SumContainer>::value)
  {
    int cnt = 0;
    for (auto&& term : m_sum)
      if (!formula::is_zero(term) && ++cnt > 1)
        return true;
  }
  else
  {
    // Derived class needs to override has_multiple_terms() const.
    assert(false);
  }
  return false;
}

template<class SumContainer>
bool Sum<SumContainer>::is_zero() const
{
  if constexpr (has_const_iterator<SumContainer>::value)
  {
    return !std::any_of(m_sum.begin(), m_sum.end(),
        [](typename SumContainer::value_type const& i)
        { return !i.is_zero(); });
  }
  else
  {
    // Derived class needs to override is_zero() const.
    assert(false);
  }
}

template<class SumContainer>
bool Sum<SumContainer>::is_unity() const
{
  if constexpr (has_const_iterator<SumContainer>::value)
  {
    auto first = m_sum.begin();
    return first != m_sum.end() && formula::is_unity(*first);
  }
  else
  {
    // Derived class needs to override is_unity() const.
    assert(false);
  }
}

//----------------------------------------------------------------------------
// Definitions of Product member functions.
//

template<class ProductContainer>
void Product<ProductContainer>::print_on(std::ostream& os) const
{
  if constexpr (has_const_iterator<ProductContainer>::value)
  {
    if (is_zero())
      os << "0";
    else if (is_unity())
      os << "1";
    else
    {
      bool first_term = true;
      auto first = m_product.begin();
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
      while (++first != m_product.end());
    }
  }
  else
  {
    // Derived class needs to override print_on(std::ostream& os) const.
    assert(false);
  }
}

template<class ProductContainer>
bool Product<ProductContainer>::starts_with_a_minus() const
{
  if constexpr (has_const_iterator<ProductContainer>::value)
  {
    bool result = false;
    std::for_each(m_product.begin(), m_product.end(),
        [&result](typename ProductContainer::value_type const& factor)
        { if (factor.starts_with_a_minus()) result = !result; });
    return result;
  }
  else
  {
    // Derived class needs to override starts_with_a_minus() const.
    assert(false);
  }
}

template<class ProductContainer>
bool Product<ProductContainer>::is_zero() const
{
  if constexpr (has_const_iterator<ProductContainer>::value)
  {
    return std::any_of(m_product.begin(), m_product.end(),
        [](typename ProductContainer::value_type const& factor)
        { return factor.is_zero(); });
  }
  else
  {
    // Derived needs to override is_zero() const.
    assert(false);
  }
}

template<class ProductContainer>
bool Product<ProductContainer>::is_unity() const
{
  if constexpr (has_const_iterator<ProductContainer>::value)
  {
    return std::all_of(m_product.begin(), m_product.end(),
        [](typename ProductContainer::value_type const& factor)
        { return factor.is_unity(); });
  }
  else
  {
    // Derived needs to override is_unity() const.
    assert(false);
  }
}

//----------------------------------------------------------------------------
// Print formula to an ostream.
//

template<class T, typename std::enable_if<is_formula<T>::value, int>::type = 0>
std::ostream& operator<<(std::ostream& os, T const& number)
{
  if (number.starts_with_a_minus())
    os << '-';
  number.print_on(os, false, false);
  return os;
}

} // namespace formula
