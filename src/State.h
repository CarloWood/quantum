#pragma once

#include "Circuit.h"
#include "InputCollector.h"
#include "EntangledState.h"
#include "debug.h"
#include <cstddef>
#include <stack>
#include <iosfwd>

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

  Circuit const* m_circuit;                             // A pointer to the underlaying circuit that this is the state of.
  std::stack<InputCollector> m_stack;                   // A temporary stack used by `apply' to parse the circuit-building code.
  std::vector<EntangledState> m_separable_states;       // A list of separable states; the Kronecker product of which forms the complete state.

 private:
  void apply(gates::GateInput const& gate_input, q_index_type chain);
  void apply(gates::GateInput const& gate_input, InputCollector const& collector);
  void apply(gates::measure const& measurement, q_index_type chain);

 public:
  State(Circuit const* circuit);

  // Called from Circuit::execute while parsing the circuit-building code.
  // chain is the current qubit index (into Circuit::m_quantum_register) and
  // current_node points to the current Circuit::Node (element of Circuit::QuBit::m_chain)
  // which is just a wrapper around a pointer to a gate input.
  int apply(q_index_type& chain, Circuit::QuBit::iterator current_node);

  // Compare if two states are equal.
  friend bool operator==(State const& lhs, State const& rhs);

  // Print the product of the m_separable_states.
  void print_on(std::ostream& os) const;
  friend std::ostream& operator<<(std::ostream& os, State const& state) { state.print_on(os); return os; }
};

} // namespace quantum
