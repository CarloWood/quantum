#include "sys.h"
#include "debug.h"
#include "formula.h"

struct ASum : public Sum
{
  std::vector<int> m_sum;

  ASum() { }
  ASum(std::initializer_list<int> il) : m_sum(il) { }

  auto begin() const { return m_sum.begin(); }
  auto end() const { return m_sum.end(); }

  ASum operator-() const;

  bool has_multiple_terms() const override { return m_sum.size() > 1; }
  bool is_zero() const override { bool has_non_zero = false; for (int e : m_sum) { if (e) { has_non_zero = true; break; } } return !has_non_zero; }
  bool is_one() const override { return m_sum.size() == 1 && ::is_one(m_sum[0]); }
  const char* plus_string() const override { return " + "; }
  const char* minus_string() const override { return " - "; }
};

struct AProduct : public Product
{
  std::vector<ASum> m_product;

  AProduct() { }
  AProduct(std::initializer_list<ASum> il) : m_product(il) { }

  auto begin() const { return m_product.begin(); }
  auto end() const { return m_product.end(); }

  AProduct operator-() const;

  bool is_zero() const override { for (auto&& e : m_product) { if (e.is_zero()) return true; } return false; }
  bool is_one() const override { return m_product.size() == 1 && ::is_one(m_product[0]); }
  const char* operator_string() const override { return " * "; }
};

struct ASum2 : public Sum
{
  std::vector<AProduct> m_sum;

  ASum2() { }
  ASum2(std::initializer_list<AProduct> il) : m_sum(il) { }

  auto begin() const { return m_sum.begin(); }
  auto end() const { return m_sum.end(); }

  ASum2 operator-() const;

  bool has_multiple_terms() const override { return m_sum.size() > 1; }
  bool is_zero() const override { bool has_non_zero = false; for (auto&& e : m_sum) { if (!e.is_zero()) { has_non_zero = true; break; } } return !has_non_zero; }
  bool is_one() const override { return m_sum.size() == 1 && ::is_one(m_sum[0]); }
  const char* plus_string() const override { return " + "; }
  const char* minus_string() const override { return " - "; }
};

ASum ASum::operator-() const
{
  ASum result;
  for (auto&& element : m_sum)
    result.m_sum.push_back(-element);
  return result;
}

AProduct AProduct::operator-() const
{
  if (m_product.empty() || is_zero())
    return *this;

  AProduct result;

  bool first = true;
  for (auto&& element : m_product)
  {
    if (first)
    {
      result.m_product.push_back(-element);
      first = false;
    }
    else
      result.m_product.push_back(element);
  }

  return result;
}

ASum2 ASum2::operator-() const
{
  ASum2 result;
  for (auto&& element : m_sum)
    result.m_sum.push_back(-element);
  return result;
}

int main()
{
  Debug(NAMESPACE_DEBUG::init());

#if 0
  ASum foo;
  foo.m_sum = {42, 1};
#else
  AProduct p1{ASum{-42, -1, -2, -3}, ASum{4, 5, 6}};
  AProduct p2{ASum{-42, 1, 2, 3}, ASum{7, 8, 9}};

  ASum2 bar{p1, p2};
#endif
  Dout(dc::notice, formula(bar));
}
