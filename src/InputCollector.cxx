#include "sys.h"
#include "InputCollector.h"
#include <iostream>

namespace quantum {

int InputCollector::chain_to_rowbit(q_index_type chain) const
{
  auto iter = m_chain_to_rowbit_map.find(chain);
  if (iter == m_chain_to_rowbit_map.end())
    return -1;
  return iter->second;
}

std::ostream& operator<<(std::ostream& os, InputCollector const& input_collector)
{
  char const* prefix = "{";
  for (auto&& m : input_collector.m_chain_to_rowbit_map)
  {
    os << prefix << m.second;
    prefix = ", ";
  }
  return os << '}';
}

} // namespace quantum
