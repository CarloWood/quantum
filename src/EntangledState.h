#pragma once

#include "QuBitField.h"
#include "Circuit.h"
#include "InputCollector.h"
#include "formula.h"
#include <vector>

namespace quantum {

class EntangledState : public formula::Sum<std::vector<QuBitField>> // List of all 2^m_number_of_quantum_bits coefficients of each product state.
{
 private:
  int m_number_of_quantum_bits;         // The number of entangled qubits that this object represents.
  std::vector<q_index_type> m_q_index;  // Maps rowbit to q_index_type.
  unsigned long m_q_index_mask;         // Has a bit set for each q_index_type in m_q_index;

 private:
  // Return the rowbit that corresponds to q_index. Only call when has(q_index) is true.
  int rowbit(q_index_type q_index) const;

 public:
  // Default constructor.
  EntangledState() : formula::Sum<std::vector<QuBitField>>{{}}, m_number_of_quantum_bits(0) { }
  // Construct an EntangledState for a single qubit in the |0⟩ state (so yeah, it isn't entangled).
  EntangledState(q_index_type quantum_register_index) :
    formula::Sum<std::vector<QuBitField>>{{1, 0}}, m_number_of_quantum_bits(1), m_q_index{quantum_register_index}, m_q_index_mask(1UL << quantum_register_index.get_value()) { }

  void merge(EntangledState const& entangled_state);

  bool has(q_index_type q_index) const { return (m_q_index_mask & (1UL << q_index.get_value())) != 0; }
  void apply(QMatrix const& matrix, q_index_type chain);

  bool has(InputCollector const& collector) const { return (m_q_index_mask & collector.q_index_mask()) != 0; }
  void apply(QMatrixX const& matrix, InputCollector const& inputs);

  void print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const;
  void print_measurement_permutations_on(std::ostream& os, Circuit const* circuit) const;

  // Accessor for m_q_index_mask.
  unsigned long q_index_mask() const { return m_q_index_mask; }

  friend bool operator!=(EntangledState const& rhs, EntangledState const& lhs);

  friend void swap(EntangledState& lhs, EntangledState& rhs);
};

} // namespace quantum
