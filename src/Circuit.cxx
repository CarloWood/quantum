#include "sys.h"
#include "Circuit.h"
#include <iostream>

namespace quantum {
namespace gates {

Standard::Standard(gate_t gate) : m_gate(gate)
{
}

CX::CX(Link const& control_link) : m_link(control_link)
{
}

co::co(Link const& control_link) : m_link(control_link)
{
}

measure::measure(int classical_bit_index) : m_classical_bit_index(classical_bit_index)
{
}

} // namespace gates

Circuit::Node::Node(gates::Standard const& gate) : m_gate(new gates::Standard(gate))
{
}

Circuit::Node::Node(gates::CX const& controlled_not) : m_gate(new gates::CX(controlled_not))
{
}

Circuit::Node::Node(gates::co const& control_link) : m_gate(new gates::co(control_link))
{
}

Circuit::Node::Node(gates::measure const& measure) : m_gate(new gates::measure(measure))
{
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::gate_t gate)
{
  m_chain.push_back(gates::Standard(gate));
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::CX const& controlled_not)
{
  m_chain.push_back(controlled_not);
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::co const& control_link)
{
  m_chain.push_back(control_link);
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::measure const& measure)
{
  m_chain.push_back(measure);
  return *this;
}

Circuit::Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits)
{
  for (size_t index = 0; index < number_of_quantum_bits; ++index)
    m_quantum_register.emplace_back(this, q_index_type{index});
  for (size_t index = 0; index < number_of_classical_bits; ++index)
    m_classical_register.emplace_back(this, c_index_type{index});
}

Circuit::QuBit& Circuit::operator[](size_t quantum_bit_index)
{
  return m_quantum_register[q_index_type{quantum_bit_index}];
}

Circuit::Result Circuit::result() const
{
  return {};
}

std::ostream& operator<<(std::ostream& os, quantum::Circuit::Result const& result)
{
  return os;
}

namespace gates {

void Standard::print_on(std::ostream& os)
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

void CX::print_on(std::ostream& os)
{
  os << "CX(" << m_link << ')';
}

void co::print_on(std::ostream& os)
{
  os << "co(" << m_link << ')';
}

void measure::print_on(std::ostream& os)
{
  os << "measure(" << m_classical_bit_index << ')';
}

} // namespace gates

void Circuit::QuBit::print_on(std::ostream& os) const
{
  os << "q[" << m_quantum_register_index << ']';
  for (auto&& node : m_chain)
    os << " << " << node;
}

std::ostream& operator<<(std::ostream& os, Circuit const& circuit)
{
  for (auto&& qubit : circuit.m_quantum_register)
    os << qubit << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, Link const& link)
{
  return os << link.m_id;
}

void Circuit::execute()
{
}

} // namespace quantum
