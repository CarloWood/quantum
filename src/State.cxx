#include "sys.h"
#include "State.h"
#include "debug.h"

namespace quantum {

State::State(Circuit const* circuit) : m_circuit(circuit)
{
  for (q_index_type q_index = circuit->q_ibegin(); q_index < circuit->q_iend(); ++q_index)
    m_separable_states.emplace_back(q_index);
}

int State::apply(q_index_type& chain, Circuit::QuBit::iterator current_node)
{
  DoutEntering(dc::qapply|continued_cf, "State::apply(" << chain << ", " << *current_node << ")... ");
  if (current_node->number_of_inputs() > 1)
  {
    InputCollector collector = m_stack.empty() ? InputCollector{} : m_stack.top();
    if (collector.have_all_inputs(chain))
    {
      Dout(dc::qapply, "Popping " << collector << " from stack.");
      m_stack.pop();                                                                                      // <--.
    }                                                                                                     //    |
    else                                                                                                  //    |
    {                                                                                                     //    |
      q_index_type next_chain = current_node->next_chain(m_circuit);                                      //    |
      assert(next_chain != chain);                                                                        //    |
      // When collector.have_all_inputs(next_chain) then we should set chain to next_chain and return -1.       |
      // This will cause this function to be called again after which it will continue here: -------------------'
      int ret = collector.have_all_inputs(next_chain) ? -1 : 1;
      if (ret == 1)
      {
        Dout(dc::qapply, "Pushing to stack: " << chain);
        m_stack.push(chain);
      }
      m_stack.top().add(chain, current_node->rowbit());
      Dout(dc::qapply, "Setting chain to " << next_chain);
      chain = next_chain;
      Dout(dc::finish, "returning " << ret << '.');
      return ret;
    }
    // Process multi-input.
    apply(current_node, collector);
    Dout(dc::finish, "returning 0.");
    return 0;
  }
  // Process single input.
  apply(current_node, chain);
  Dout(dc::finish, "returning 0.");
  return 0;
}

void State::apply(Circuit::QuBit::iterator node, q_index_type chain)
{
  Dout(dc::notice, "Applying " << *node << " to qubit " << chain << '.');
  for (auto entangled_state = m_separable_states.begin(); entangled_state != m_separable_states.end(); ++entangled_state)
    if (entangled_state->has(chain))
    {
      entangled_state->apply(node->gate_input().matrix(), chain);
      break;
    }
}

void State::apply(Circuit::QuBit::iterator node, InputCollector const& collector)
{
  Dout(dc::notice, "Applying " << *node << " to qubits " << collector << '.');
  auto entangled_state = m_separable_states.begin();
  while (!entangled_state->has(collector))
    ++entangled_state;
  auto const first_entangled_state = entangled_state;
  Dout(dc::notice, "Found state " << *first_entangled_state);
  auto new_end_entangled_state = m_separable_states.end();
  while (++entangled_state != new_end_entangled_state)
    if (entangled_state->has(collector))
    {
      Dout(dc::notice, "Merging state " << *entangled_state);
      first_entangled_state->merge(*entangled_state);
      Dout(dc::notice, "Merged state " << *first_entangled_state);
      if (entangled_state == --new_end_entangled_state)
        break;
      std::swap(*entangled_state, *new_end_entangled_state);
    }
  first_entangled_state->apply(node->gate_input().matrixX(), collector);
  m_separable_states.erase(new_end_entangled_state, m_separable_states.end());
}

void State::print_on(std::ostream& os) const
{
  char const* prefix = "";
  for (auto entangled_state : m_separable_states)
  {
    os << prefix << entangled_state;
    prefix = " \u2297 ";
  }
}

} // namespace quantum

#if defined(CWDEBUG) && !defined(DOXYGEN)
NAMESPACE_DEBUG_CHANNELS_START
channel_ct qapply("QAPPLY");
NAMESPACE_DEBUG_CHANNELS_END
#endif
