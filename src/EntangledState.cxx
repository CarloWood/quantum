#include "sys.h"
#include "EntangledState.h"
#include "utils/is_power_of_two.h"
#include "utils/BitSet.h"
#include "utils/reversed.h"
#include <iostream>
#include <set>

namespace quantum {

int EntangledState::rowbit(q_index_type q_index) const
{
  int rowbit = 0;
  for (auto qi : m_q_index)
  {
    if (qi == q_index)
      return rowbit;
    ++rowbit;
  }
  // Don't call this function unless EntangledState::has(q_index) returns true.
  ASSERT(false);
  return -1;
}

void EntangledState::apply(QMatrix const& matrix, q_index_type chain)
{
  Eigen::IOFormat MatLabFmt(Eigen::FullPrecision, 0, " ", ";", "", "", "[", "]");
//  DoutEntering(dc::notice, "EntangledState::apply(" << matrix.format(MatLabFmt) << ", " << chain << ")");
  unsigned long rowbit_mask = 1UL << rowbit(chain);
  unsigned long coefficients = 1UL << m_number_of_quantum_bits;
  for (unsigned long i0 = 0; i0 < coefficients; ++i0)
  {
    if ((i0 & rowbit_mask))
      continue; // Already handled as i1.
    unsigned long i1 = i0 | rowbit_mask;
    QVector v1{m_sum[i0], m_sum[i1]};
    QVector v2{matrix * v1};
    m_sum[i0] = v2[0];
    m_sum[i1] = v2[1];
  }
  Dout(dc::notice, "Result: " << *this);
}

void EntangledState::apply(QMatrixX const& matrix, InputCollector const& inputs)
{
  Eigen::IOFormat MatLabFmt(Eigen::FullPrecision, 0, " ", ";", "", "", "[", "]");
//  DoutEntering(dc::notice, "EntangledState::apply(" << matrix.format(MatLabFmt) << ", " << inputs << ")");

  unsigned long const number_of_matrix_product_states = matrix.cols();  // The number of product states that the matrix works on;
  assert(utils::is_power_of_two(number_of_matrix_product_states));      // the matrix works on log2(number_of_matrix_product_states) qubits.

  int const number_of_inputs = inputs.number_of_inputs();
  // The number of inputs must match the size of the matrix.
  assert(number_of_matrix_product_states == 1UL << number_of_inputs);

  unsigned long const number_of_entangled_product_states = 1UL << m_number_of_quantum_bits;
  // Matrix can't work on more qubits than this EntangledState contains.
  assert(number_of_matrix_product_states <= number_of_entangled_product_states);

  // Consider a gate with three inputs (i0, i1 and i2) that
  // respectively are connected to qubit/chain n2, n3 and n1.
  //                      .
  //       .----------> q[n1]-- . . . ---+----------------------
  //       |              .              |         .------.             Having number_of_inputs = 3 inputs, the matrix will be NxN large,
  //       |              .              |         |      |             where N = number_of_matrix_product_states = 2^3 = 8.
  //       | .--------> q[n2]-- . . . -------------i0     o-----
  //       | |            .              |         |      |
  //       | |            .              |    .----i1     |
  //       | |            .              |    |    |      |
  //       | |            .              `---------i2     |
  //       | |            .                   |    |      |
  //       | |            .                   |    `------'
  //      .--|--------> q[n3]-- . . . --------+-----------------
  //      vv v            .
  //      43210 <-- rowbit
  //     |00000⟩
  //     |00001⟩  Also consider an EntangledState of five qubits, 2^5 = 32 product states,
  //     |00010⟩  where (row)bits 4, 3 and 1 correspond respectively to chain n3, n1 and n2.
  //     |00011⟩  Lets call those three (row)bits the "used" bits.
  //        .     The other two (row)bits correspond to other qubit/chains that are not used
  //        .     as intput for the gate and will be called the "unused" bits.
  //        .
  //     |11111⟩  Having five rowbits, the EntangledState contains the coefficients of
  //              number_of_entangled_product_states = 2^5 = 32 product states.
  //
  // All coefficients must be changed as follows:
  //
  // For each fixed value of the unused rowbits (2^2 = 4 values in total here),
  // the 3 used rowbits must be mapped to the index of a vector with size number_of_matrix_product_states
  // by mapping the bit that corresponds to i0 to the least significant bit, the bit that corresponds
  // to i1 to the next bit and so on. Thus, in this case, the value of rowbit 1 is used as least
  // significant bit, then the value of rowbit 4 and rowbit 3 is used as most significant bit:
  //
  //     vector_row =  rowbit3 * 4 + rowbit4 * 2 + rowbit1
  //
  // And each such vector must be operated upon by matrix.
  // Hence, using the product state to represent its coefficient,
  // for this example case we need the matrix to work on:
  //
  //          unused bits: 0 0      0 1      1 0      1 1
  // row (in binary):      v v      v v      v v      v v
  //            000   | |00000⟩  |00001⟩  |00100⟩  |00101⟩ |
  //            001   | |00010⟩  |00011⟩  |00110⟩  |00111⟩ |
  //            010   | |10000⟩  |10001⟩  |10100⟩  |10101⟩ |
  //            011   | |10010⟩  |10011⟩  |10110⟩  |10111⟩ |
  //            100   | |01000⟩  |01001⟩  |01100⟩  |01101⟩ |
  //            101   | |01010⟩  |01011⟩  |01110⟩  |01111⟩ |
  //            110   | |11000⟩  |11001⟩  |11100⟩  |11101⟩ |
  //            111   | |11010⟩  |11011⟩  |11110⟩  |11111⟩ |
  //            ^^^      ^^ ^     ^^ ^     ^^ ^     ^^ ^
  //            210      12 0     12 0     12 0     12 0

  // In order to generate the above matrix (or four vectors, one for each column),
  // we start with generating a vector of size m_number_of_quantum_bits (5) and
  // fill that with the following values (in binary): 00010, 10000, 01000, 00001 and 00100.
  // Where the last 2 are the unused bits and the first number_of_inputs (3) are the used
  // bits in the order i0, i1, i2.
  unsigned long used_mask = 0;
  unsigned long unused_mask = 0;
  std::vector<unsigned long> masks(m_number_of_quantum_bits);
  for (int entangled_rowbit = 0; entangled_rowbit < m_number_of_quantum_bits; ++entangled_rowbit)
  {
    int matrix_rowbit = inputs.chain_to_rowbit(m_q_index[entangled_rowbit]);
    if (matrix_rowbit != -1)    // Is this a used bit?
    {
      masks[matrix_rowbit] = 1UL << entangled_rowbit;
      used_mask |= 1UL << entangled_rowbit;             // Eventually becomes 11010.
    }
    else
      unused_mask |= 1UL << entangled_rowbit;           // Eventually becomes 00101.
  }
  // Add the unused bits too.
  int extended_matrix_rowbit = number_of_inputs;
  for (int entangled_rowbit = 0; entangled_rowbit < m_number_of_quantum_bits; ++entangled_rowbit)
  {
    unsigned long mask = 1UL << entangled_rowbit;
    if ((unused_mask & mask))
      masks[extended_matrix_rowbit++] = mask;
  }

  // Temporary copy for coefficients.
  Eigen::Matrix<QuBitField, Eigen::Dynamic, 1> v1;
  v1.resize(number_of_matrix_product_states);

  // This allows us to generate the index into m_sum starting at the top of the first
  // column and then going down first and left to right second.
  unsigned long vi = 0;
  for (unsigned long i = 0;;)
  {
    unsigned long mask;
    // Copy coefficients to the temporary vector.
    v1[vi++] = m_sum[i];
    vi %= number_of_matrix_product_states;
    // Do we have a complete vector/column?
    if (vi == 0)
    {
      // Apply matrix on this vector and store the result in v2.
      Eigen::Matrix<QuBitField, Eigen::Dynamic, 1> v2;
      v2.resize(number_of_matrix_product_states);
      v2 = matrix * v1;
      // Copy v2 back to m_sum.
      unsigned long i2 = i & ~used_mask;        // Reset to top of the column.
      for (unsigned long vi2 = 0; vi2 < number_of_matrix_product_states; ++vi2)
      {
        m_sum[i2] = v2[vi2];
        // "Increment" i2.
        int j = 0;
        while (j < m_number_of_quantum_bits && ((mask = masks[j]) & i2))
        {
          i2 ^= mask;
          ++j;
        }
        if (j == m_number_of_quantum_bits)
          break;
        i2 ^= masks[j];
      }
    }
    // "Increment" i.
    int j = 0; // Index into masks[];
    while (j < m_number_of_quantum_bits && ((mask = masks[j]) & i))
    {
      i ^= mask;
      ++j;
    }
    if (j == m_number_of_quantum_bits)
      break;
    i ^= masks[j];
  }
  Dout(dc::notice, "Result: " << *this);
}

static std::array<char const*, 10> subscript = { "\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089" };

std::string subscript_str(int val)
{
  std::string result;
  if (val > 9)
  {
    result = subscript_str(val / 10);
    val %= 10;
  }
  result += subscript[val];
  return result;
}

void print_subscript_on(std::ostream& os, int val)
{
  if (val > 9)
  {
    print_subscript_on(os, val / 10);
    val %= 10;
  }
  os << subscript[val];
}

void EntangledState::merge(EntangledState const& entangled_state)
{
  for (q_index_type q_index : entangled_state.m_q_index)
    m_q_index.push_back(q_index);
  unsigned long const rowbit_mod = 1UL << m_number_of_quantum_bits;
  m_number_of_quantum_bits += entangled_state.m_number_of_quantum_bits;
  unsigned long const number_of_states = 1UL << m_number_of_quantum_bits;
  std::vector<QuBitField> new_coef;
  for (unsigned long si = 0; si < number_of_states; ++si)
    new_coef.push_back(m_sum[si % rowbit_mod] * entangled_state.m_sum[si / rowbit_mod]);
  m_sum.swap(new_coef);
  m_q_index_mask |= entangled_state.m_q_index_mask;
}

void EntangledState::print_on(std::ostream& os, bool negate_all_terms, bool is_factor) const
{
  unsigned long const number_of_product_states = 1 << m_number_of_quantum_bits;
  if (number_of_product_states == 1)
  {
    os << 'I';
    return;
  }
  //--------------------------------------------------------------------------
  // This is a copy of Sum::print_on, with some adjustments marked with a *).
  bool const starts_with_a_minus_ = starts_with_a_minus() != negate_all_terms;
  bool const has_multiple_terms_ = has_multiple_terms();
  bool const needs_parens = has_multiple_terms_ && is_factor;
  bool const toggle_sign_all_terms = (needs_parens && starts_with_a_minus_) != negate_all_terms;
  if (needs_parens)
    os << '(';
  bool first_term = true;
  auto first = m_sum.begin();
  unsigned long state = 0;      // *) Added.
  do
  {
    if (!formula::is_zero(*first))
    {
      if (!first_term)
        os << (formula::starts_with_a_minus(*first) != toggle_sign_all_terms ? " - " : " + ");
#if 1                           // *) Added.
      if (first->is_unity())
        os << '|';
      else
      {
#endif
        print_formula_on(*first, os, toggle_sign_all_terms, true /* *) was false */);
#if 1                           // *) Added.
        os << "\u00b7|"; // "·|"
      }
      for (int i = m_number_of_quantum_bits - 1; i >= 0; --i)
      {
        int mask = 1 << i;
        os << ((state & mask) ? '1' : '0');
        print_subscript_on(os, m_q_index[i].get_value());
      }
      os << "\u27e9"; // "⟩"
#endif
      first_term = false;
    }
    ++state;                    // * Added.
  }
  while (++first != m_sum.end());
  if (needs_parens)
    os << ')';
  //--------------------------------------------------------------------------
}

class MeasurementEntangledProductState : public formula::Product<std::array<QuBitField, 1>>
{
 private:
  std::map<int, char> m_product_state;

 public:
  MeasurementEntangledProductState(QuBitField const& amplitude, std::map<int, char>&& product_state) :
    formula::Product<std::array<QuBitField, 1>>({ amplitude }), m_product_state(std::move(product_state)) { }

  bool starts_with_a_minus() const override { return m_product[0].starts_with_a_minus(); }
  bool is_zero() const override { return m_product[0].is_zero(); }
  bool is_unity() const override { return false; }
  void print_on(std::ostream& os) const
  {
    m_product[0].print_on(os, false, true);
    os << "\u00b7|"; // "·|"
    for (auto&& z_state : adaptor::reversed(m_product_state))
      os << z_state.second + subscript_str(z_state.first);
    os << "\u27e9"; // "⟩"
    os << "\tChance: " << (m_product[0] * m_product[0].conjugate());
  }
};

class MeasurementEntangledSubState : public formula::Sum<std::vector<MeasurementEntangledProductState>>
{
 public:
  // Construct an empty State, with no product states yet :).
  MeasurementEntangledSubState() : formula::Sum<std::vector<MeasurementEntangledProductState>>({}) { }

  void add(QuBitField const& amplitude, std::map<int, char>&& product_state) { m_sum.emplace_back(amplitude, std::move(product_state)); }
};

void EntangledState::print_measurement_permutations_on(std::ostream& os, Circuit const* circuit) const
{
  unsigned long const all_measurements_mask = circuit->get_measurement_mask();
  using measurement_mask_type = utils::BitSet<unsigned long>;   // A bitset of measurement bits.
  using measurement_bit_type = utils::bitset::Index;            // The index of a single measurement bit.
  // For example, suppose we have 19 qubits and 10 measurement bits:
  //
  //                        22222222211111111110000000000 } -- q_index runs from 0 to 28.
  //               q_index: 87654321098765432109876543210 }
  // all_measurements_mask: 11111111110000000000000000000   -- The 10 high qubits are the measurement bits.
  //        m_q_index_mask: 00100100010001010001000011000   -- The qubits involved in this EntangledState.
  //      measurement_mask: 00100100010000000000000000000   -- The measurement bits involved in this EntangledState.
  //                rowbit:   7  2   3   0 6   1    54      -- 'Abitrary' mapping. Rowbit here runs from 0 to 15, and when
  //                                                           used as index into m_q_index[rowbit] we get the q_index.
  // The vector measurement_bits will contain:
  //             index:0    00000100000000000000000000000
  //                   1    00000000010000000000000000000
  //                   2    00100000000000000000000000000

  // The measurement bits that take part in this EntangledState.
  measurement_mask_type const measurement_mask{m_q_index_mask & all_measurements_mask};

  using rowbit_mask_type = utils::BitSet<unsigned long>;        // A bitset of rowbit bits.
  using rowbit_type = utils::bitset::Index;                     // The index of a single rowbit bit (usually just called 'rowbit' despite that it is an index).
  // Hence, a row (for this EntangledState matrix) has number_of_rowbits = 8 bits:
  //                   rowbit: 76543210                         // rowbit runs from 0 (rowbit_begin) to 7 (rowbit_end - 1).
  // rowbit_measurements_mask: 10001100   -- Only bits 2, 3 and 7 correspond to measurement bits.

  // The total number of rowbits of this EntangledState.
  int8_t const number_of_rowbits = m_number_of_quantum_bits;

  // The rowbit range.
  rowbit_type const rowbit_begin = utils::bitset::index_begin;
  rowbit_type const rowbit_end = utils::bitset::IndexPOD{number_of_rowbits};

  // Convert the measurement bits in m_q_index to a vector of measurement_bit_type.
  std::vector<std::pair<measurement_bit_type, rowbit_type>> measurement_bits;   // A list of the q_index (as measurement_bit_type) / rowbit index
                                                                                // pairs of measurement bits involved in this EntangledState.
  rowbit_mask_type rowbit_measurements_mask;                    // A mask with all of those bits.
  rowbit_measurements_mask.reset();
  for (rowbit_type rowbit{rowbit_begin}; rowbit != rowbit_end; ++rowbit)
  {
    // Convert the rowbit into the corresponding q_index, as measurement_bit_type.
    int8_t const quantum_register_index = m_q_index[rowbit()].get_value();
    measurement_bit_type const q_index = utils::bitset::IndexPOD{quantum_register_index};
    // When it is actually a measurement bit (and not a normal qubit), store it:
    if (measurement_mask.test(q_index))
    {
      rowbit_measurements_mask.set(rowbit);
      measurement_bits.emplace_back(q_index, rowbit);
    }
  }

  using perm_mask_type = utils::BitSet<unsigned long>;          // A bitset of permutation bits.
  using perm_type = utils::bitset::Index;                       // The index of a single permutation bit.
  // And the permutation thereof has 3 (number_of_measurement_bits) bits:
  //                              732  -- Corresponding rowbit
  //                              210  -- A perm runs from 0 (perm_begin) to 2 (perm_end - 1).
  // measurement_bit_permutation: 000  00000000 <-- corresponding measurement_bits_permutation_mask
  //                              001  00000100
  //                              010  00001000
  //                              011  00001100
  //                              100  10000000
  //                              101  10000100
  //                              110  10001000
  //                              111  10001100

  // The number of rowbits that correspond to measurement bits, in this EntangledState.
  int8_t const number_of_measurement_bits = measurement_bits.size();

  // The perm range.
  perm_type const perm_begin = utils::bitset::index_begin;
  perm_type const perm_end = utils::bitset::IndexPOD{number_of_measurement_bits};

  // The row range.
  rowbit_mask_type const row_begin{rowbit_mask_type::mask_type(0)};
  rowbit_mask_type const row_end{rowbit_mask_type::mask_type(1) << number_of_rowbits};

  // Run over all measurement permutations.
  perm_mask_type const measurement_bit_permutation_end{perm_mask_type::mask_type(1) << number_of_measurement_bits};
  perm_mask_type measurement_bit_permutation{perm_mask_type::mask_type(0)};
  for (perm_mask_type measurement_bit_permutation{perm_mask_type::mask_type(0)};
       measurement_bit_permutation != measurement_bit_permutation_end;
       ++measurement_bit_permutation)
  {
    std::map<int, char> classical_bits;
    rowbit_mask_type measurement_bits_permutation_mask;
    measurement_bits_permutation_mask.reset();
    for (perm_type i{perm_begin}; i != perm_end; ++i)
    {
      char c;
      if (measurement_bit_permutation.test(i))
      {
        measurement_bits_permutation_mask.set(measurement_bits[i()].second);
        c = '1';
      }
      else
        c = '0';
      size_t measurement_bit_q_index_val = measurement_bits[i()].first();
      q_index_type measurement_bit_q_index(measurement_bit_q_index_val);
      classical_bits[circuit->classical_index(measurement_bit_q_index)] = c;
    }
    std::string prefix = "\n";
    for (auto&& classical_bit : adaptor::reversed(classical_bits))
      prefix += classical_bit.second + subscript_str(classical_bit.first);
    prefix += ": ";
    MeasurementEntangledSubState mess;
    // Run over all possible rows to find those that correspond with this permutation.
    for (rowbit_mask_type row{row_begin}; row != row_end; ++row)
    {
      // Only consider the product states of this measurement permutation.
      if ((row & rowbit_measurements_mask) == measurement_bits_permutation_mask)
      {
        // Don't print anything if it doesn't exist.
        if (m_sum[row()].is_zero())
          continue;
        std::map<int, char> product_state;
        for (rowbit_type rowbit{rowbit_begin}; rowbit != rowbit_end; ++rowbit)
        {
          char c = row.test(rowbit) ? '1' : '0';
          int8_t const quantum_register_index = m_q_index[rowbit()].get_value();
          measurement_bit_type const q_index = utils::bitset::IndexPOD{quantum_register_index};
          // When it is not a measurement bit, store it:
          if (!measurement_mask.test(q_index))
            product_state[quantum_register_index] = c;
        }
        mess.add(m_sum[row()], std::move(product_state));
      }
    }
    os << prefix << mess;
  }
}

void swap(EntangledState& lhs, EntangledState& rhs)
{
  std::swap(lhs.m_number_of_quantum_bits, rhs.m_number_of_quantum_bits);
  std::swap(lhs.m_q_index_mask, rhs.m_q_index_mask);
  std::swap(lhs.m_sum, rhs.m_sum);
  std::swap(lhs.m_q_index, rhs.m_q_index);
}

bool operator!=(EntangledState const& lhs, EntangledState const& rhs)
{
  // This shouldn't be called when the number of quantum bits differ;
  // that is dangerous as it might still be equal after merging the
  // EntangledState with another first.
  assert(lhs.m_number_of_quantum_bits == rhs.m_number_of_quantum_bits);
  // Same thing.
  assert(lhs.m_q_index_mask == rhs.m_q_index_mask);
  // Sorry, not implemented yet.
  assert(lhs.m_q_index == rhs.m_q_index);
  return lhs.m_sum != rhs.m_sum;
}

} // namespace quantum
