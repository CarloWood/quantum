#include "sys.h"
#include "EntangledState.h"
#include "utils/is_power_of_two.h"
#include <iostream>

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
    QVector v1{m_coef[i0], m_coef[i1]};
    QVector v2{matrix * v1};
    m_coef[i0] = v2[0];
    m_coef[i1] = v2[1];
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

  // This allows us to generate the index into m_coef starting at the top of the first
  // column and then going down first and left to right second.
  unsigned long vi = 0;
  for (unsigned long i = 0;;)
  {
    unsigned long mask;
    // Copy coefficients to the temporary vector.
    v1[vi++] = m_coef[i];
    vi %= number_of_matrix_product_states;
    // Do we have a complete vector/column?
    if (vi == 0)
    {
      // Apply matrix on this vector and store the result in v2.
      Eigen::Matrix<QuBitField, Eigen::Dynamic, 1> v2;
      v2.resize(number_of_matrix_product_states);
      v2 = matrix * v1;
      // Copy v2 back to m_coef.
      unsigned long i2 = i & ~used_mask;        // Reset to top of the column.
      for (unsigned long vi2 = 0; vi2 < number_of_matrix_product_states; ++vi2)
      {
        m_coef[i2] = v2[vi2];
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

void print_subscript_on(std::ostream& os, int val)
{
  if (val > 9)
  {
    print_subscript_on(os, val / 10);
    val %= 10;
  }
  static std::array<char const*, 10> substript = { "\u2080", "\u2081", "\u2082", "\u2083", "\u2084", "\u2085", "\u2086", "\u2087", "\u2088", "\u2089" };
  os << substript[val];
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
    new_coef.push_back(m_coef[si % rowbit_mod] * entangled_state.m_coef[si / rowbit_mod]);
  m_coef.swap(new_coef);
  m_q_index_mask |= entangled_state.m_q_index_mask;
}

void EntangledState::print_on(std::ostream& os, bool need_parens) const
{
#if 0
  unsigned long const number_of_product_states = 1 << m_number_of_quantum_bits;
  if (number_of_product_states == 1)
  {
    os << 'I';
    return;
  }
  char const* prefix = "";
  bool multiple_product_states = need_parens;
  if (need_parens)
  {
    int cnt = 0;
    for (unsigned long state = 0; state < number_of_product_states; ++state)
      if (m_coef[state] != 0)
        if (cnt++ > 1)
          break;
    if (cnt == 1)
      multiple_product_states = false;
  }
  if (need_parens && multiple_product_states)
    os << '(';
  for (unsigned long state = 0; state < number_of_product_states; ++state)
  {
    if (m_coef[state] == 0)
      continue;
    os << prefix;
    prefix = " + ";
    if (m_coef[state] == -1)
      os << '-';
    else if (m_coef[state] != 1)
    {
      bool has_multiple_terms;
      std::stringstream ss;
      FormulaPrinter<QuBitField>(m_coef[state]).print_on(ss, has_multiple_terms);
      if (has_multiple_terms)
        os << '(';
      os << ss.str();
      if (has_multiple_terms)
        os << ')';
    }
    os << "\u00b7|"; // "·|"
    for (int i = m_number_of_quantum_bits - 1; i >= 0; --i)
    {
      int mask = 1 << i;
      os << ((state & mask) ? '1' : '0');
      print_subscript_on(os, m_q_index[i].get_value());
    }
    os << "\u27e9"; // "⟩"
    prefix = " + ";
  }
  if (need_parens && multiple_product_states)
    os << ')';
#endif
  os << "FIXME";
}

void swap(EntangledState& lhs, EntangledState& rhs)
{
  std::swap(lhs.m_number_of_quantum_bits, rhs.m_number_of_quantum_bits);
  std::swap(lhs.m_q_index_mask, rhs.m_q_index_mask);
  std::swap(lhs.m_coef, rhs.m_coef);
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
  return lhs.m_coef != rhs.m_coef;
}

} // namespace quantum
