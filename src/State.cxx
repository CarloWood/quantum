#include "sys.h"
#include "State.h"
#include "debug.h"

namespace quantum {

State::State(Circuit const* circuit) : m_circuit(circuit)
{
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
    Dout(dc::notice|flush_cf, "Applying " << *current_node << " to qubits " << collector << '.');
    // FIXME: implement
    Dout(dc::finish, "returning 0.");
    return 0;
  }
  // Process single input.
  Dout(dc::notice, "Applying " << *current_node << " to qubit " << chain << '.');
  // FIXME: implement
  Dout(dc::finish, "returning 0.");
  return 0;
}

} // namespace quantum

#if defined(CWDEBUG) && !defined(DOXYGEN)
NAMESPACE_DEBUG_CHANNELS_START
channel_ct qapply("QAPPLY");
NAMESPACE_DEBUG_CHANNELS_END
#endif
