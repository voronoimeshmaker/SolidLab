
// -----------------------------------------------------------------------------
// tests/SolidLab/test_solidlab.cpp
// Unit tests for the SolidLab class.
// Build with: -DBUILD_TESTS=ON (root CMake brings GoogleTest via FetchContent)
// -----------------------------------------------------------------------------
//
// Test layout:
//  - Version: semantic-like format "X.Y.Z".
//  - Notes: basic CRUD-ish behaviours (size, access, out_of_range).
//  - Join: formatting with default and custom separators, empty cases.
//
// Notes for students:
//  - Prefer small, intention-revealing tests.
//  - Each TEST block should document "Given/When/Then" implicitly.
//  - Avoid hidden coupling between tests (fresh fixture per TEST).
// -----------------------------------------------------------------------------

#include <gtest/gtest.h>
#include "SolidLab.hpp"

#include <algorithm>   // std::count
#include <numeric>     // std::accumulate
#include <string>
#include <vector>
#include <stdexcept>

using solidlab::SolidLab;

/// Count a given char in a string (tiny helper to assert "X.Y.Z" shape).
static std::size_t count_char(std::string_view s, char c) {
    return static_cast<std::size_t>(std::count(s.begin(), s.end(), c));
}

// ---- Version ----------------------------------------------------------------

/**
 * @test Version string should be non-empty and contain two dots,
 *       resembling a semantic-like "MAJOR.MINOR.PATCH".
 */
TEST(SolidLab_Version, NonEmptyAndSemanticLike) {
    const auto v = SolidLab::version();
    EXPECT_FALSE(v.empty());
    EXPECT_EQ(count_char(v, '.'), 2u);
}

// ---- Notes: size & content ---------------------------------------------------

/**
 * @test A fresh instance starts empty.
 */
TEST(SolidLab_Notes, StartsEmpty) {
    SolidLab lab;
    EXPECT_EQ(lab.size(), 0u);
}

/**
 * @test Adding notes increases size and preserves insertion order/content.
 */
TEST(SolidLab_Notes, AddNoteIncreasesSizeAndStoresContent) {
    SolidLab lab;
    lab.add_note("alpha");
    EXPECT_EQ(lab.size(), 1u);
    EXPECT_EQ(lab.note(0), "alpha");

    lab.add_note("beta");
    EXPECT_EQ(lab.size(), 2u);
    EXPECT_EQ(lab.note(1), "beta");
}

/**
 * @test Access out of range throws std::out_of_range.
 *       We explicitly discard the [[nodiscard]] return value.
 */
TEST(SolidLab_Notes, OutOfRangeThrows) {
    SolidLab lab;
    lab.add_note("only");
    EXPECT_THROW((void)lab.note(1), std::out_of_range);  // invalid index -> throw
    EXPECT_NO_THROW((void)lab.note(0));                  // valid index -> ok
}

// ---- Join -------------------------------------------------------------------

/**
 * @test Joining an empty container returns an empty string for any separator.
 */
TEST(SolidLab_Join, EmptyWhenNoNotes) {
    SolidLab lab;
    EXPECT_EQ(lab.join(), "");
    EXPECT_EQ(lab.join('-'), "");
}

/**
 * @test Default separator is comma ','.
 */
TEST(SolidLab_Join, DefaultSeparatorIsComma) {
    SolidLab lab;
    lab.add_note("one");
    lab.add_note("two");
    lab.add_note("three");
    EXPECT_EQ(lab.join(), "one,two,three");
}

/**
 * @test Custom separator should be placed between items without trailing sep.
 */
TEST(SolidLab_Join, CustomSeparatorWorks) {
    SolidLab lab;
    lab.add_note("a");
    lab.add_note("b");
    lab.add_note("c");
    EXPECT_EQ(lab.join('-'), "a-b-c");
    EXPECT_EQ(lab.join(' '), "a b c");
}

/**
 * @test Join length equals sum of item lengths plus separators (N-1).
 */
TEST(SolidLab_Join, LengthMatchesSumOfPartsPlusSeparators) {
    SolidLab lab;
    std::vector<std::string> data = {"red", "green", "blue", "x"};
    for (const auto& s : data) lab.add_note(s);

    const char sep = ';';
    const auto joined = lab.join(sep);

    const std::size_t parts =
        static_cast<std::size_t>(std::accumulate(
            data.begin(), data.end(), 0u,
            [](std::size_t acc, const std::string& s) {
                return acc + static_cast<std::size_t>(s.size());
            }));

    const std::size_t expected_len =
        parts + (data.size() > 0 ? data.size() - 1 : 0);

    EXPECT_EQ(joined.size(), expected_len);
    EXPECT_NE(joined.find(sep), std::string::npos);
}
