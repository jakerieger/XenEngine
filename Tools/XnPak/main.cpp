// Author: Jake Rieger
// Created: 10/24/2024.
//

#include "XnPak.hpp"

#include <CLI/CLI.hpp>
#include <Panic.hpp>

int main(int argc, char* argv[]) {
    CLI::App app("XenEngine asset packing tool.", "XnPak");
    argv = app.ensure_utf8(argv);

    str manifestFile = "Content.manifest";  // Default is relative to cwd
    app.add_option("-m,--manifest", manifestFile, "Manifest file describing how to pack assets.")
      ->required();

    auto versionCallback = [&]() {
        std::cout << app.get_display_name() << " version "
                  << "0.0.1-dev" << std::endl;
        std::exit(0);
    };
    app.add_flag_callback("-v,--version", versionCallback, "Display app version information.");

    static bool build = false;
    static bool clean = false;
    const auto buildCmd =
      app.add_subcommand("build", "Build manifest")->callback([&]() { build = true; });
    const auto cleanCmd =
      app.add_subcommand("clean", "Clean manifest")->callback([&]() { clean = true; });
    app.require_subcommand(1);  // Require exactly one subcommand
    buildCmd->group("BuildAction");
    cleanCmd->group("BuildAction");

    CLI11_PARSE(app, argc, argv);

    Xen::Manifest manifest(manifestFile);
    if (build) {
        manifest.build();
    } else if (clean) {
        manifest.clean();
    } else {
        Panic("I'm honestly not sure how you even reached this point.");
    }

    return 0;
}