#include "sys.h"
#include "Circuit.h"

namespace quantum {
namespace gates {

CX::CX(Link const& control_link)
{
}

co::co(Link const& control_link)
{
}

measure::measure(int classical_bit_index)
{
}

} // namespace gates

Circuit::QuBit& Circuit::QuBit::operator<<(gates::gate_t gate)
{
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::CX const& controlled_not)
{
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::co const& control_link)
{
  return *this;
}

Circuit::QuBit& Circuit::QuBit::operator<<(gates::measure const& measure)
{
  return *this;
}

Circuit::Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits)
{
}

Circuit::QuBit Circuit::operator[](int quantum_bit_index)
{
  return QuBit{};
}

Circuit::Result Circuit::result() const
{
  return {};
}

std::ostream& operator<<(std::ostream& os, quantum::Circuit::Result const& result)
{
  return os;
}

} // namespace quantum
