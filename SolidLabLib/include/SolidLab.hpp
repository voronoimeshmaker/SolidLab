#pragma once
/**
 * @file SolidLab.hpp
 * @brief Primary public class for the SolidLab training library.
 * @details Minimal API to support early TDD and examples.
 */

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace solidlab {

/**
 * @class SolidLab
 * @brief Small façade used across examples and tests.
 *
 * Design notes:
 *  - Keep default and copy constructors available.
 *  - Header stays light; implementation lives in .cpp.
 *  - No external deps; STL only.
 */
class SolidLab {
public:
    // ---- Ctors / dtor (rule of zero) ----
    SolidLab() = default;
    SolidLab(const SolidLab&) = default;
    SolidLab& operator=(const SolidLab&) = default;
    ~SolidLab() = default;

    /// @brief Library semantic version (MAJOR.MINOR.PATCH).
    [[nodiscard]] static std::string version();

    /// @brief Adds a short note to the internal collection.
    void add_note(std::string_view text);

    /// @brief Number of stored notes.
    [[nodiscard]] std::size_t size() const noexcept;

    /// @brief Access a stored note (bounds-checked).
    /// @throws std::out_of_range if idx >= size().
    [[nodiscard]] const std::string& note(std::size_t idx) const;

    /// @brief Joins all notes with a separator.
    [[nodiscard]] std::string join(char sep = ',') const;

private:
    std::vector<std::string> notes_;
};

}  // namespace solidlab

