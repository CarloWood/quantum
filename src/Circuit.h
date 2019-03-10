#pragma once

#include "Gates.h"
#include "QuBit.h"
#include "utils/Vector.h"
#include <iosfwd>
#include <memory>

namespace quantum {

namespace index_category {
enum qubits {};         // Index into quantum register.
enum clbits {};         // Index into classical register.
} // namespace category

class Link
{
 private:
  int m_id;

 public:
  Link(int id) : m_id(id) { }

  int get_id() const { return m_id; }

  friend std::ostream& operator<<(std::ostream& os, Link const& link);
};

namespace gates {

class Base
{
 public:
  virtual void print_on(std::ostream& os) = 0;
};

class Standard : public Base
{
 private:
  gate_t m_gate;

 public:
  Standard(gate_t gate);
  void print_on(std::ostream& os) override;
};

class CX : public Base
{
 private:
  Link m_link;

 public:
  CX(Link const& control_link);
  void print_on(std::ostream& os) override;
};

class co : public Base
{
 private:
  Link m_link;

 public:
  co(Link const& control_link);
  void print_on(std::ostream& os) override;
};

class measure : public Base
{
 private:
  int m_classical_bit_index;

 public:
  measure(int classical_bit_index);
  void print_on(std::ostream& os) override;
};

} // namespace gates

class Circuit
{
 private:
  using q_index_type = utils::VectorIndex<index_category::qubits>;
  using c_index_type = utils::VectorIndex<index_category::clbits>;

  class Node
  {
   private:
    std::shared_ptr<gates::Base> m_gate;

   public:
    Node(gates::Standard const& gate);
    Node(gates::CX const& controlled_not);
    Node(gates::co const& control_link);
    Node(gates::measure const& measure);

    friend std::ostream& operator<<(std::ostream& os, Node const& node)
    {
      node.m_gate->print_on(os);
      return os;
    }
  };

  class QuBit
  {
   private:
    quantum::QuBit m_qubit;
    Circuit* m_circuit;
    q_index_type m_quantum_register_index;
    std::vector<Node> m_chain;

   public:
    QuBit(Circuit* circuit, q_index_type quantum_register_index) : m_qubit(Z0), m_circuit(circuit), m_quantum_register_index(quantum_register_index) { }

    QuBit& operator<<(gates::gate_t gate);
    QuBit& operator<<(gates::CX const& controlled_not);
    QuBit& operator<<(gates::co const& control_link);
    QuBit& operator<<(gates::measure const& measure);

    void print_on(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, QuBit const& qubit)
    {
      qubit.print_on(os);
      return os;
    }
  };

  class Bit
  {
   private:
    bool m_bit;
    Circuit* m_circuit;
    c_index_type m_classical_register_index;

   public:
    Bit(Circuit* circuit, c_index_type classical_register_index) : m_bit(false), m_circuit(circuit), m_classical_register_index(classical_register_index) { }
  };

  utils::Vector<QuBit, q_index_type> m_quantum_register;
  utils::Vector<Bit, c_index_type> m_classical_register;

 public:
  class Result
  {
   public:
    friend std::ostream& operator<<(std::ostream& os, Result const& result);
  };

  Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits);

  QuBit& operator[](size_t quantum_bit_index);

  void execute();
  Result result() const;

  friend std::ostream& operator<<(std::ostream& os, Circuit const& circuit);
};

} // namespace quantum
