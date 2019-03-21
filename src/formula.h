#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <functional>

template<typename T>
T negate(T const& val)
{
  return -val;
}

struct Sum
{
  virtual bool has_multiple_terms() const = 0;
  virtual bool is_zero() const = 0;
  virtual bool is_one() const = 0;
  virtual char const* plus_string() const = 0;
  virtual char const* minus_string() const = 0;
};

struct Product
{
  virtual bool is_zero() const = 0;
  virtual bool is_one() const = 0;
  virtual char const* operator_string() const = 0;
};

template<typename T, typename std::enable_if<std::is_base_of<Sum, T>::value, int>::type = 0>
bool has_multiple_terms(T const& val)
{
  return val.has_multiple_terms();
}

template<typename T, typename std::enable_if<!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value, int>::type = 0>
bool is_zero(T const& val)
{
  return val == 0;
}

template<typename T, typename std::enable_if<std::is_base_of<Sum, T>::value || std::is_base_of<Product, T>::value, int>::type = 0>
bool is_zero(T const& val)
{
  return val.is_zero();
}

template<typename T, typename std::enable_if<!std::is_base_of<Sum, T>::value && !std::is_base_of<Product, T>::value, int>::type = 0>
bool is_one(T const& val)
{
  return val == 1;
}

template<typename T, typename std::enable_if<std::is_base_of<Sum, T>::value || std::is_base_of<Product, T>::value, int>::type = 0>
bool is_one(T const& val)
{
  return val.is_one();
}

template<typename T>
struct FormulaPrinter
{
  T const& m_val;

  FormulaPrinter(T const& val) : m_val(val) { }

  std::string to_string(bool& has_multiple_terms, bool& negated, std::function<int(bool)> const& extra_length) const;
  std::string to_string(bool& has_multiple_terms) const;

  friend std::ostream& operator<<(std::ostream& os, FormulaPrinter const& formula_printer)
  {
    bool has_multiple_terms = false;
    bool negated = false;
    std::string str = formula_printer.to_string(has_multiple_terms, negated, [](bool has_multiple_terms){ return has_multiple_terms ? 3 : 1; });
    if (negated)
    {
      os << '-';
      if (has_multiple_terms)
        os << '(';
    }
    os << str;
    if (negated && has_multiple_terms)
      os << ')';
    return os;
  }
};

template<typename T>
FormulaPrinter<T> formula(T const& val)
{
  return {val};
}

template<typename T>
std::string FormulaPrinter<T>::to_string(bool& has_multiple_terms, bool& negated, std::function<int(bool)> const& extra_length) const
{
  std::string str1 = to_string(has_multiple_terms);
  bool has_multiple_terms2;
  std::string str2 = FormulaPrinter<T>(negate(m_val)).to_string(has_multiple_terms2);
  bool removes_beginning_minus = (str1[0] == '-' || str1[0] == '!') && (str2[0] != '-' && str2[0] != '!');
  if (str2.size() + extra_length(has_multiple_terms2) - (removes_beginning_minus ? 1 : 0) < str1.size())
  {
    negated = true;
    has_multiple_terms = has_multiple_terms2;
    return str2;
  }
  negated = false;
  return str1;
}

template<typename T>
std::string FormulaPrinter<T>::to_string(bool& has_multiple_terms) const
{
  if constexpr (std::is_base_of<Sum, T>::value)
  {
    std::string str;
    int non_zero_terms = 0;
    bool first = true;
    for (auto&& element : m_val)
    {
      if (is_zero(element))
        continue;
      ++non_zero_terms;
      bool negated;
      std::string element_str = formula(element).to_string(has_multiple_terms, negated, [first](bool has_multiple_terms){ return (first ? 1 : 0) + (has_multiple_terms ? 2 : 0); });
      bool needs_parens = negated && has_multiple_terms;
      if (first)
      {
        first = false;
        if (negated)
          str = '-';
        else
          ++non_zero_terms;     // Make sure it is larger than 1.
      }
      else
        str += negated ? m_val.minus_string() : m_val.plus_string();
      if (needs_parens)
        str += '(';
      str += element_str;
      if (needs_parens)
        str += ')';
    }
    has_multiple_terms = non_zero_terms > 1;
    if (non_zero_terms == 0)
      str = "0";
    return str;
  }
  else if constexpr (std::is_base_of<Product, T>::value)
  {
    std::string str;
    bool first = true;
    int non_one_factors = 0;
    for (auto&& element : m_val)
    {
      if (is_one(element))
        continue;
      ++non_one_factors;
    }
    bool has_multiple_factors = non_one_factors > 1;
    for (auto&& element : m_val)
    {
      if (is_one(element))
        continue;
      bool negated;
      std::string element_str = formula(element).to_string(has_multiple_terms, negated, [](bool){ return 1; });
      bool needs_parens = has_multiple_factors && has_multiple_terms;
      if (first)
        first = false;
      else
        str += m_val.operator_string();
      if (negated)
        str += '-';
      if (needs_parens)
        str += '(';
      str += element_str;
      if (needs_parens)
        str += ')';
    }
    has_multiple_terms = false;
    return str;
  }
  else
  {
    has_multiple_terms = false;
    std::stringstream ss;
    ss << m_val;
    return ss.str();
  }
}
