// Author: Jake Rieger
// Created: 11/14/2024.
//

#include "Asset.hpp"
#include "AssetManifest.hpp"
#include "BuildCache.hpp"
#include "Processors.inl"
#include "Project.hpp"

#include <CLI/CLI.hpp>

int main(int argc, char* argv[]) {
    CLI::App app("XEN Engine asset management and packing tool.", "XPak");
    argv = app.ensure_utf8(argv);

    str projectFilename;
    app.add_option("-p,--project", projectFilename, "Path to project (*.xproj) file")->required();
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
      app.add_subcommand("build", "Build project content.")->callback([&]() { build = true; });
    auto* rebuildCmd = app.add_subcommand("rebuild", "Rebuild project content.")->callback([&]() {
        rebuild = true;
    });
    auto* cleanCmd =
      app.add_subcommand("clean", "Clean project content.")->callback([&]() { clean = true; });

    app.require_subcommand(1);
    buildCmd->group("Action");
    rebuildCmd->group("Action");
    cleanCmd->group("Action");

    CLI11_PARSE(app, argc, argv);

    const Project project(projectFilename);

    if (build) {
        std::cout << "Building project..." << std::endl;
        project.Build();
    } else if (rebuild) {
        std::cout << "Rebuilding project..." << std::endl;
        project.Rebuild();
    } else if (clean) {
        std::cout << "Cleaning project..." << std::endl;
        project.Clean();
    }

    return 0;
}