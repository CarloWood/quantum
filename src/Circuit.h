#pragma once

#include "Gates.h"
#include <iosfwd>

namespace quantum {

class Link
{
};

namespace gates {

class CX
{
 public:
  CX(Link const& control_link);
};

class co
{
 public:
  co(Link const& control_link);
};

class measure
{
 public:
  measure(int classical_bit_index);
};

} // namespace gates

class Circuit
{
 private:
  class QuBit
  {
    public:
     QuBit& operator<<(gates::gate_t gate);
     QuBit& operator<<(gates::CX const& controlled_not);
     QuBit& operator<<(gates::co const& control_link);
     QuBit& operator<<(gates::measure const& measure);
  };

 public:
  class Result
  {
   public:
    friend std::ostream& operator<<(std::ostream& os, Result const& result);
  };

  Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits);

  QuBit operator[](int quantum_bit_index);
  Result result() const;
};

} // namespace quantum
