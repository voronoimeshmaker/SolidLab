// Minimal example for SolidLab.
// Build with: -DBUILD_EXAMPLES=ON
// The executable will be placed next to this source file.

#include "SolidLab.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    using solidlab::SolidLab;

    std::cout << "SolidLab v" << SolidLab::version() << "\n";

    SolidLab lab;
    lab.add_note("alpha");
    lab.add_note("beta");
    lab.add_note("gamma");

    std::cout << "Notes count: " << lab.size() << "\n";
    std::cout << "First note: " << lab.note(0) << "\n";

    // Optional: first CLI char defines the join separator.
    char sep = (argc > 1 && argv[1][0] != '\0') ? argv[1][0] : '-';
    std::cout << "Joined: " << lab.join(sep) << "\n";

    // Demonstrate bounds checking.
    try {
        (void)lab.note(999);  // will throw
    } catch (const std::out_of_range& e) {
        std::cout << "Caught std::out_of_range\n";
    }

    return 0;
}

