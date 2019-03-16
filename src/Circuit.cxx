#include "sys.h"
#include "Circuit.h"
#include "State.h"
#include <iostream>
#include <vector>

namespace quantum {

Circuit::Node::Node(gates::Standard const& gate) : m_gate_input(new gates::Standard(gate))
{
}

Circuit::Node::Node(gates::CX const& controlled_not) : m_gate_input(new gates::CX(controlled_not))
{
}

Circuit::Node::Node(gates::co const& control_link) : m_gate_input(new gates::co(control_link))
{
}

Circuit::Node::Node(gates::measure const& measure) : m_gate_input(new gates::measure(measure))
{
}

Circuit::QuBit& Circuit::QuBit::operator-(gates::gate_t gate)
{
  m_chain.push_back(gates::Standard(gate));
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator-(gates::CX const& controlled_not)
{
  m_circuit->saw(controlled_not, m_quantum_register_index);
  m_chain.push_back(controlled_not);
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator-(gates::co const& control_link)
{
  m_circuit->saw(control_link, m_quantum_register_index);
  m_chain.push_back(control_link);
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator-(gates::measure const& measure)
{
  m_chain.push_back(measure);
  return *this;
}

void Circuit::saw(gates::ControlledNOT const& input, q_index_type quantum_register_index)
{
  auto res = m_map[input.rowbit()].insert(map_type::value_type{input.id(), quantum_register_index});
#ifdef CWDEBUG
  if (!res.second)
  {
    std::stringstream ss;
    input.print_on(ss);
    DoutFatal(dc::core, "Duplicate " << ss.str() << "(" << input.id() << ")");
  }
#endif
  // Only use each id once.
  assert(res.second);
}

Circuit::Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits) : m_state(nullptr)
{
  for (size_t index = 0; index < number_of_quantum_bits; ++index)
    m_quantum_register.emplace_back(this, q_index_type{index});
  for (size_t index = 0; index < number_of_classical_bits; ++index)
    m_classical_register.emplace_back(this, c_index_type{index});
}

Circuit::~Circuit()
{
  delete m_state;
}

Circuit::QuBit& Circuit::operator[](size_t quantum_bit_index)
{
  return m_quantum_register[q_index_type{quantum_bit_index}];
}

Circuit::Result Circuit::result() const
{
  return {m_state};
}

std::ostream& operator<<(std::ostream& os, quantum::Circuit::Result const& result)
{
  result.m_state->print_on(os);
  return os;
}

namespace gates {

QMatrix const& Standard::matrix() const
{
  return gate[m_gate];
}

void Standard::print_on(std::ostream& os) const
{
  switch (m_gate)
  {
    case X:
      os << "X";
      break;
    case Y:
      os << "Y";
      break;
    case Z:
      os << "Z";
      break;
    case S:
      os << "S";
      break;
    case S_inv:
      os << "S_inv";
      break;
    case T:
      os << "T";
      break;
    case T_inv:
      os << "T_inv";
      break;
    case H:
      os << "H";
      break;
  }
}

void CX::print_on(std::ostream& os) const
{
  os << "CX(" << m_id << ')';
}

void co::print_on(std::ostream& os) const
{
  os << "co(" << m_id << ')';
}

QMatrix const& measure::matrix() const
{
  Dout(dc::warning, "Calling measure::matrix()");
  return gates::I;
}

void measure::print_on(std::ostream& os) const
{
  os << "measure(" << m_classical_bit_index << ')';
}

} // namespace gates

void Circuit::QuBit::print_on(std::ostream& os) const
{
  os << "q[" << m_quantum_register_index << ']';
  for (auto&& node : m_chain)
    os << " - " << node;
}

std::ostream& operator<<(std::ostream& os, Circuit const& circuit)
{
  for (auto&& qubit : circuit.m_quantum_register)
    os << qubit << std::endl;
  return os;
}

namespace gates {

q_index_type ControlledNOT::next_chain(Circuit const* circuit, int rowbit) const
{
  return circuit->next_chain(m_id, rowbit);
}

} // namespace gates

// `id' is the id of a multi-input gate that we just ran into on the current chain.
// This function needs to return the chain on which a still missing input resides.
q_index_type Circuit::next_chain(int id, int rowbit) const
{
  DoutEntering(dc::qapply, "Circuit::next_chain(" << id << ", " << rowbit << ")");
  assert(rowbit < (ssize_t)m_map.size());
  assert(m_map.size() == 2);      // Haven't implemented support for more than 2 inputs yet.
#ifdef CWDEBUG
  Dout(dc::qapply, "Elements of m_map:");
  int i = 0;
  for (auto&& a_map : m_map)
  {
    Dout(dc::qapply, "m_map[" << i << "] =");
    for (auto&& element : a_map)
      Dout(dc::qapply, "  " << element.first << " --> " << element.second);
    ++i;
  }
#endif
  return m_map[1 - rowbit].at(id);
}

void Circuit::execute()
{
  size_t const number_of_quantum_bits = m_quantum_register.size();
  //size_t const number_of_classical_bits = m_classical_register.size();

  // Create the circuit matrix.
  m_state = new State(this);

  // Initialize all qubit pointers.
  utils::Vector<QuBit::iterator, q_index_type> node;
  utils::Vector<QuBit::iterator, q_index_type> end;
  for (auto&& qubit : m_quantum_register)
  {
    node.push_back(qubit.begin());
    end.push_back(qubit.end());
  }

  size_t const first_qubit = 0;
  q_index_type chain{first_qubit};
  q_index_type const last{number_of_quantum_bits};
  while (chain != last)
  {
    int total_switched_chain = 0;
    while (node[chain] != end[chain])
    {
      q_index_type prev_chain = chain;
      int switched_chain = m_state->apply(chain, node[chain]);  // Might change `chain'.
      total_switched_chain += switched_chain;
      if (switched_chain != 1)
        ++node[prev_chain];
    }
    assert(total_switched_chain == 0);
    ++chain;
  }
}

} // namespace quantum
