// Author: Jake Rieger
// Created: 10/24/24.
//

#pragma once

#include <cstdarg>
#include <iostream>

namespace Xen {
    [[noreturn]] static void
    panic_impl(const char* file, int line, const char* func, const char* fmt, ...) noexcept {
        va_list args;
        va_start(args, fmt);
        char message[1024];
        vsnprintf(message, sizeof(message), fmt, args);
        va_end(args);
        std::cerr << file << "(" << line << ") :: PANIC\n"
                  << "  In: " << func << "\n"
                  << "  Message: " << message << std::endl;
        std::abort();
    }

#ifndef Panic
    /// @brief Terminates the program, displaying the provided error message as well as the file,
    /// line, and function name where the panic occurred.
    #define Panic(fmt, ...) Xen::panic_impl(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif
}  // namespace Xen