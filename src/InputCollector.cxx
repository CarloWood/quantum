#include "sys.h"
#include "InputCollector.h"
#include <iostream>

namespace quantum {

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
