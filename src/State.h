#pragma once

#include "Circuit.h"
#include "InputCollector.h"
#include "EntangledState.h"
#include "debug.h"
#include <cstddef>
#include <stack>

#if defined(CWDEBUG) && !defined(DOXYGEN)
NAMESPACE_DEBUG_CHANNELS_START
extern channel_ct qapply;
NAMESPACE_DEBUG_CHANNELS_END
#endif

namespace quantum {

// Quantum state of the whole system.
class State
{
 private:
  using q_index_type = utils::VectorIndex<index_category::qubits>;
  using c_index_type = utils::VectorIndex<index_category::clbits>;

  Circuit const* m_circuit;                             // A pointer to the underlaying circuit that this is the state of.
  std::stack<InputCollector> m_stack;                   // A temporary stack used by `apply' to parse the circuit-building code.
  std::vector<EntangledState> m_separable_states;       // A list of separable states; the Kronecker product of which forms the complete state.

 private:
  void apply(Circuit::QuBit::iterator node, q_index_type chain);
  void apply(Circuit::QuBit::iterator node, InputCollector const& collector);

 public:
  State(Circuit const* circuit);

  // Called from Circuit::execute while parsing the circuit-building code.
  // chain is the current qubit index (into Circuit::m_quantum_register) and
  // current_node points to the current Circuit::Node (element of Circuit::QuBit::m_chain)
  // which is just a wrapper around a pointer to a gate input.
  int apply(q_index_type& chain, Circuit::QuBit::iterator current_node);

  // Print the product of the m_separable_states.
  void print_on(std::ostream& os) const;
};

} // namespace quantum
