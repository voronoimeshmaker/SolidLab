/**
 * @file SolidLab.cpp
 * @brief Implementation of the SolidLab class.
 */

#include "SolidLab.hpp"

#include <stdexcept>

namespace solidlab {

std::string SolidLab::version() {
    // Keep it simple; you can wire this to CMake later if you prefer.
    return "1.0.0";
}

void SolidLab::add_note(std::string_view text) {
    notes_.emplace_back(text);
}

std::size_t SolidLab::size() const noexcept {
    return notes_.size();
}

const std::string& SolidLab::note(std::size_t idx) const {
    return notes_.at(idx);  // throws std::out_of_range if invalid
}

std::string SolidLab::join(char sep) const {
    if (notes_.empty()) return {};

    // Precompute capacity to reduce reallocations.
    std::size_t total = 0;
    for (const auto& s : notes_) total += s.size();
    total += (notes_.size() - 1);  // separators

    std::string out;
    out.reserve(total);

    for (std::size_t i = 0; i < notes_.size(); ++i) {
        out += notes_[i];
        if (i + 1 < notes_.size()) out += sep;
    }
    return out;
}

}  // namespace solidlab

