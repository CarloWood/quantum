#pragma once

#include "Gates.h"
#include "QuBit.h"
#include "utils/Vector.h"
#include <iosfwd>
#include <memory>
#include <map>

namespace quantum {

namespace index_category {
enum qubits {};         // Index into quantum register.
} // namespace category

class Circuit;

using q_index_type = utils::VectorIndex<index_category::qubits>;

namespace gates {

// Base class for gate inputs.
class GateInput
{
 public:
  virtual ~GateInput() { }
  virtual int number_of_inputs() const = 0;
  virtual bool is_measurement() const { return false; }
  virtual int rowbit() const { assert(false); }
  virtual q_index_type next_chain(Circuit const* UNUSED_ARG(circuit), int UNUSED_ARG(rowbit)) const { assert(false); }
  virtual QMatrix const& matrix() const = 0;
  virtual QMatrixX const& matrixX() const { assert(false); }
  virtual void print_on(std::ostream& os) const = 0;

  friend std::ostream& operator<<(std::ostream& os, GateInput const& gate_input) { gate_input.print_on(os); return os; }
};

template<int n>
class MultiInput : public GateInput
{
  int number_of_inputs() const override { return n; }
  QMatrix const& matrix() const override { assert(false); }
};

using SingleInput = MultiInput<1>;

// Base class for Controlled-NOT inputs.
class ControlledNOT : public MultiInput<2>
{
 protected:
  int m_id;     // Link id of the contolled NOT; or - if this is a measurement - the classical bit index.

 private:
  int number_of_inputs() const override { return 2; }
  q_index_type next_chain(Circuit const* circuit, int rowbit) const override;
  QMatrixX const& matrixX() const override { return gates::Controlled_X; }

 public:
  ControlledNOT(int id) : m_id(id) { }
  int id() const { return m_id; }
};

// Controlled-NOT -- control input.
class co : public ControlledNOT
{
 private:
  // The control bit usually corresponds with first qubit,
  // as in |1⟩ ⊗ |0⟩ = |10⟩.
  //        ^           ^
  //        |           |
  //     control        |
  //       bit           \.
  //                      \.
  // And hence, with the more significant bit of the two.
  //
  // This way the required matrix representation can stay
  // the same as what everyone uses, see for example
  // https://en.wikipedia.org/wiki/Controlled_NOT_gate#Operation
  //
  // Note however that since we print the lowest qubit indices
  // first, this then corresponds with:
  //
  // ----(+)-----
  //      |
  // -----o------
  //
  // instead of the image that is usually associated with
  // a CNOT gate (with the control input at the top).
  int rowbit() const override { return 1; }
  void print_on(std::ostream& os) const override;

 public:
  using ControlledNOT::ControlledNOT;
};

// Controlled-NOT -- manipulated input.
class CX : public ControlledNOT
{
 private:
  // The output bit is usually drawn on the bottom, which in our case corresponds with a higher qubit index.
  int rowbit() const override { return 0; }
  void print_on(std::ostream& os) const override;

 public:
  using ControlledNOT::ControlledNOT;
};

// A standard gate (X, Y, Z, S, S_inv, T, T_inv and H).
class Standard : public SingleInput
{
 private:
  gate_t m_gate;

  QMatrix const& matrix() const override;
  void print_on(std::ostream& os) const override;

 public:
  Standard(gate_t gate) : m_gate(gate) { }
};

// Measurement -- entanglement with a measurement bit (two realities in one).
class measure : public ControlledNOT
{
 private:
  size_t m_number_of_quantum_bits;      // Copy of Circuit::m_number_of_quantum_bits (after the call to apply_offset).

  void print_on(std::ostream& os) const override;
  bool is_measurement() const override { return true; }

  measure(int classical_bit_index, size_t number_of_quantum_bits) : ControlledNOT(classical_bit_index), m_number_of_quantum_bits(number_of_quantum_bits) { }

 public:
  measure(int classical_bit_index) : ControlledNOT(classical_bit_index), m_number_of_quantum_bits(0) { }
  measure apply_offset(size_t number_of_quantum_bits) const { return {m_id, number_of_quantum_bits}; }
  q_index_type get_q_index() const { return q_index_type{m_number_of_quantum_bits + m_id}; }
};

} // namespace gates

class State;

class Circuit
{
 private:
  friend class State;

  class Node
  {
   private:
    std::shared_ptr<gates::GateInput> m_gate_input;

   public:
    Node(gates::Standard const& gate);
    Node(gates::CX const& controlled_not);
    Node(gates::co const& control_link);
    Node(gates::measure const& measure);

    int number_of_inputs() const { return m_gate_input->number_of_inputs(); }
    q_index_type next_chain(Circuit const* circuit) const { return m_gate_input->next_chain(circuit, m_gate_input->rowbit()); }
    int rowbit() const { return m_gate_input->rowbit(); }
    bool is_measurement() const { return m_gate_input->is_measurement(); }
    gates::GateInput const& gate_input() const { return *m_gate_input; }

    friend std::ostream& operator<<(std::ostream& os, Node const& node)
    {
      node.m_gate_input->print_on(os);
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

    QuBit& operator-(gates::gate_t gate);
    QuBit& operator-(gates::CX const& controlled_not);
    QuBit& operator-(gates::co const& control_link);
    QuBit& operator-(gates::measure const& measure);

    void print_on(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, QuBit const& qubit)
    {
      qubit.print_on(os);
      return os;
    }

    using iterator = std::vector<Node>::iterator;
    iterator begin() { return m_chain.begin(); }
    iterator end() { return m_chain.end(); }

    bool is_measurement() const { return m_quantum_register_index.get_value() >= m_circuit->m_number_of_quantum_bits; }
  };

  using quantum_register_type = utils::Vector<QuBit, q_index_type>;
  using map_type = std::map<int, q_index_type>;

  size_t m_number_of_quantum_bits;              // The real number of quantum bits.
  quantum_register_type m_quantum_register;     // A QuBit for each quantum bit and one for each classical bit.
  std::shared_ptr<State> m_state;
  std::array<map_type, 2> m_map;

 private:
  void saw(gates::ControlledNOT const& input, q_index_type quantum_register_index);

 public:
  class Result
  {
   private:
    std::shared_ptr<State const> m_state;

   public:
    Result(std::shared_ptr<State const> state) : m_state(state) { }
    friend std::ostream& operator<<(std::ostream& os, Result const& result);
  };

  Circuit(size_t number_of_quantum_bits, size_t number_of_classical_bits) { reset(number_of_quantum_bits, number_of_classical_bits); }
  void reset(size_t number_of_quantum_bits, size_t number_of_classical_bits);

  QuBit& operator[](size_t quantum_bit_index);

  q_index_type q_ibegin() const { return m_quantum_register.ibegin(); }
  q_index_type q_iend() const { return m_quantum_register.iend(); }
  q_index_type next_chain(int id, int rowbit) const;
  unsigned long get_measurement_mask() const { return (1UL << m_quantum_register.size()) - (1UL << m_number_of_quantum_bits); }
  bool is_measurement(q_index_type q_index) const { return q_index.get_value() >= m_number_of_quantum_bits; }
  size_t classical_index(q_index_type q_index) const { return q_index.get_value() - m_number_of_quantum_bits; }

  void execute();
  std::shared_ptr<State> state() const;
  Result result() const;

  friend std::ostream& operator<<(std::ostream& os, Circuit const& circuit);
};

} // namespace quantum
