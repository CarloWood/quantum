#pragma once

#include <iosfwd>
#include <string>

namespace quantum {

// Circular basis, { |↻⟩, |↺⟩ }.
enum YState {
  Y0,   // |↻⟩
  Y1    // |↺⟩
};

std::string to_string(YState state);
std::ostream& operator<<(std::ostream& os, quantum::YState state);

} // namespace quantum
