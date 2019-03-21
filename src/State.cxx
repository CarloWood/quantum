#include "sys.h"
#include "State.h"
#include "debug.h"
#include "utils/reversed.h"
#include <algorithm>
#include <numeric>

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
  Dout(dc::notice, "State now: " << *this);
  return 0;
}

void State::apply(Circuit::QuBit::iterator node, q_index_type chain)
{
  DoutEntering(dc::notice, "State::apply(" << *node << ", " << chain << ')');
  for (auto entangled_state = m_separable_states.begin(); entangled_state != m_separable_states.end(); ++entangled_state)
    if (entangled_state->has(chain))
    {
      entangled_state->apply(node->gate_input().matrix(), chain);
      break;
    }
  Dout(dc::notice, "State now: " << *this);
}

void State::apply(Circuit::QuBit::iterator node, InputCollector const& collector)
{
  DoutEntering(dc::notice, "State::apply(" << *node << ", " << collector << ')');
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
  Dout(dc::notice, "State now: " << *this);
}

void State::print_on(std::ostream& os) const
{
  bool const need_parens = m_separable_states.size() > 1;
  char const* prefix = "";
  for (auto entangled_state : adaptor::reversed(m_separable_states))
  {
    os << prefix;
    entangled_state.print_on(os, need_parens);
    prefix = " \u2297 ";        // " ⊗ "
  }
}

// Only use for debugging purposes; this isn't *always* exact.
bool operator==(State const& lhs, State const& rhs)
{
  size_t lhs_size = lhs.m_separable_states.size();
  std::vector<int> lhs_i(lhs_size);
  std::iota(lhs_i.begin(), lhs_i.end(), 0);
  std::sort(lhs_i.begin(), lhs_i.end(), [&](int a, int b){ return lhs.m_separable_states[a].q_index_mask() < lhs.m_separable_states[b].q_index_mask(); });
  size_t rhs_size = rhs.m_separable_states.size();
  std::vector<int> rhs_i(rhs_size);
  std::iota(rhs_i.begin(), rhs_i.end(), 0);
  std::sort(rhs_i.begin(), rhs_i.end(), [&](int a, int b){ return rhs.m_separable_states[a].q_index_mask() < rhs.m_separable_states[b].q_index_mask(); });
  std::vector<int>::iterator lhs_ii = lhs_i.begin();
  std::vector<int>::iterator rhs_ii = rhs_i.begin();
  for (;;)
  {
    unsigned long lhs_mask = lhs.m_separable_states[*lhs_ii].q_index_mask();
    unsigned long rhs_mask = rhs.m_separable_states[*rhs_ii].q_index_mask();
    if ((lhs_mask & rhs_mask) != std::min(lhs_mask, rhs_mask))
      return false;     // Different entangled states. Strictly this could mean that both sides holds an EntangledState that is actually separable,
                        // ie, XY and YZ; where the correct (most separated) list should be X, Y, Z, but to detect that I'd have to always calculate
                        // the full state (merge all EntangledState's) and I don't want to do that (or worse, be able to find those separations).
    if (lhs_mask == rhs_mask)
    {
      if (lhs.m_separable_states[*lhs_ii] != rhs.m_separable_states[*rhs_ii])
        return false;
    }
    else
    {
      unsigned long larger_mask = lhs_mask | rhs_mask;
      State const& smaller_hsp{(lhs_mask < rhs_mask) ? lhs : rhs};
      State const& larger_hsp{(lhs_mask > rhs_mask) ? lhs : rhs};
      std::vector<int>::iterator& smaller_hs_ii{(lhs_mask < rhs_mask) ? lhs_ii : rhs_ii};
      std::vector<int>::iterator& larger_hs_ii{(lhs_mask > rhs_mask) ? lhs_ii : rhs_ii};
      std::vector<int>::iterator const& smaller_ii_end{(lhs_mask < rhs_mask) ? lhs_i.end() : rhs_i.end()};
      EntangledState smaller_es_cpy{smaller_hsp.m_separable_states[*smaller_hs_ii]};
      while (smaller_hs_ii != smaller_ii_end && smaller_es_cpy.q_index_mask() < larger_mask)
        smaller_es_cpy.merge(smaller_hsp.m_separable_states[*++smaller_hs_ii]);
      if (smaller_hs_ii == smaller_ii_end || smaller_es_cpy.q_index_mask() != larger_mask)
        return false;
      if (larger_hsp.m_separable_states[*larger_hs_ii] != smaller_es_cpy)
        return false;
    }
    ++lhs_ii;
    ++rhs_ii;
    if (lhs_ii == lhs_i.end() || rhs_ii == rhs_i.end())
      return lhs_ii == lhs_i.end() && rhs_ii == rhs_i.end();
  }
}

} // namespace quantum

#if defined(CWDEBUG) && !defined(DOXYGEN)
NAMESPACE_DEBUG_CHANNELS_START
channel_ct qapply("QAPPLY");
NAMESPACE_DEBUG_CHANNELS_END
#endif
