// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <CLI/CLI.hpp>

int main(int argc, char* argv[]) {
    CLI::App app("XEN Engine asset management and packing tool.", "XPak");
    argv = app.ensure_utf8(argv);

    CLI11_PARSE(app, argc, argv);

    return 0;
}