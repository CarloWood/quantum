#pragma once

#include <iosfwd>
#include <string>

namespace quantum {

// Computational basis, { |0⟩, |1⟩ }.
enum ZState {
  Z0,   // |0⟩
  Z1    // |1⟩
};

std::string to_string(ZState state);
std::ostream& operator<<(std::ostream& os, quantum::ZState state);

} // namespace quantum
