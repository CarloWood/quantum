#pragma once

#include <iosfwd>
#include <string>

namespace quantum {

// Superposition basis, { |+⟩, |−⟩ }.
enum XState {
  X0,   // |+⟩
  X1    // |−⟩
};

std::string to_string(XState state);
std::ostream& operator<<(std::ostream& os, quantum::XState state);

} // namespace quantum
