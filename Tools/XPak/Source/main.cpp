// Author: Jake Rieger
// Created: 11/14/2024.
//

#include "Asset.hpp"
#include "Manifest.hpp"
#include "BuildCache.hpp"
#include "Processors.inl"

#include <CLI/CLI.hpp>

int main(int argc, char* argv[]) {
    CLI::App app("XEN Engine asset management and packing tool.", "XPak");
    argv = app.ensure_utf8(argv);

    str manifestFilename;
    app.add_option("-m,--manifest", manifestFilename, "Path to manifest file")->required();
    app.add_flag_callback(
      "-v, --version",
      [&]() {
          std::cout << app.get_display_name() << " version 0.0.1-dev" << std::endl;
          std::exit(0);
      },
      "Display app version information.");

    bool build   = false;
    bool rebuild = false;
    bool clean   = false;

    auto* buildCmd =
      app.add_subcommand("build", "Build manifest.")->callback([&]() { build = true; });
    auto* rebuildCmd =
      app.add_subcommand("rebuild", "Rebuild manifest.")->callback([&]() { rebuild = true; });
    auto* cleanCmd =
      app.add_subcommand("clean", "Clean manifest.")->callback([&]() { clean = true; });

    app.require_subcommand(1);
    buildCmd->group("Action");
    rebuildCmd->group("Action");
    cleanCmd->group("Action");

    CLI11_PARSE(app, argc, argv);

    Manifest manifest(manifestFilename);

    if (build) {
        std::cout << "Building manifest..." << std::endl;
        manifest.Build();
    } else if (rebuild) {
        std::cout << "Rebuilding manifest..." << std::endl;
        manifest.Rebuild();
    } else if (clean) {
        std::cout << "Clean manifest..." << std::endl;
        manifest.Clean();
    }

    return 0;
}