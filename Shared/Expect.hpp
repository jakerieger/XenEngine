// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#include <iostream>
#include <optional>
#include <ostream>
#include <string>

/// @brief Rust-style expect for optional types. Terminates program on null value with exit code
/// '-10'.
template<typename T>
T Expect(const std::optional<T>& opt, const std::string& message) {
    if (!opt.has_value()) {
        std::cerr << "Expect: " << message << std::endl;
        std::exit(-10);
    }
    return *opt;
}