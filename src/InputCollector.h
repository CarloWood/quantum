#pragma once

#include "Circuit.h"
#include <iosfwd>

namespace quantum {

// Store for a mapping from row bit (index) to qubit index.
// A row bit index is, for example for a gate with three inputs:
//
//  210 <-- Row bit index, 0, 1 or 2.
//  000
//  001
//  010
//  011
//  100
//  101
//  110
//  111
//  ^^^__ three bit wide entangled state.
//
class InputCollector
{
 private:
  using chain_to_rowbit_map_type = std::map<int, q_index_type>;
  q_index_type m_start_chain;                           // This gate was first encountered on this chain.
  chain_to_rowbit_map_type m_chain_to_rowbit_map;       // Map row bit (index) to qubit index.

 public:
  InputCollector() { }
  InputCollector(q_index_type start_chain) : m_start_chain(start_chain) { }

  // Return true iff all inputs where collected (default constructed object will return false).
  bool have_all_inputs(q_index_type& current_chain) const { return current_chain == m_start_chain; }

  // Remember which qubit (chain) corresponds to which (matrix) row bit.
  void add(q_index_type chain, int rowbit) { m_chain_to_rowbit_map[rowbit] = chain; }

  friend std::ostream& operator<<(std::ostream& os, InputCollector const& input_collector);
};

} // namespace quantum
