#pragma once

#include "QuBitField.h"
#include "Circuit.h"
#include "InputCollector.h"
#include <vector>

namespace quantum {

class EntangledState
{
 private:
  int m_number_of_quantum_bits;         // The number of entangled qubits that this object represents.
  std::vector<QuBitField> m_coef;       // List of all 2^m_number_of_quantum_bits coefficients of each product state.
  std::vector<q_index_type> m_q_index;  // Maps rowbit to q_index_type.
  unsigned long m_q_index_mask;         // Has a bit set for each q_index_type in m_q_index;

 private:
  // Return the rowbit that corresponds to q_index. Only call when has(q_index) is true.
  int rowbit(q_index_type q_index) const;

 public:
  // Default constructor.
  EntangledState() : m_number_of_quantum_bits(0) { }
  // Construct an EntangledState for a single qubit in the |0> state (so yeah, it isn't entangled).
  EntangledState(q_index_type quantum_register_index) :
    m_number_of_quantum_bits(1), m_coef{1, 0}, m_q_index{quantum_register_index}, m_q_index_mask(1UL << quantum_register_index.get_value()) { }

  void merge(EntangledState const& entangled_state);

  bool has(q_index_type q_index) const { return (m_q_index_mask & (1UL << q_index.get_value())) != 0; }
  void apply(QMatrix const& matrix, q_index_type chain);

  bool has(InputCollector const& collector) const { return (m_q_index_mask & collector.q_index_mask()) != 0; }
  void apply(QMatrixX const& matrix, InputCollector const& inputs);

  friend std::ostream& operator<<(std::ostream& os, EntangledState const& entangle_state);
  friend void swap(EntangledState& lhs, EntangledState& rhs);
};

} // namespace quantum
