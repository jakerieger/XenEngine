// Author: Jake Rieger
// Created: 11/27/2024.
//

#include "Compression.hpp"
#include "Expect.hpp"
#include "IO.hpp"

#include <Types.hpp>
#include <CLI/CLI.hpp>
#include <pugixml.hpp>
#include <filesystem>
#include <vector>
#include <iostream>

/// @brief Defines the structure of a XEN project file (.xproj).
struct XenProject {
    str Name;
    str Version;
    str Author;
    str ContentDirectory;
    str ScenesDirectory;
    str ScriptsDirectory;
};

static void BuildProject(const XenProject& project) {
    std::cout << "Building project " << project.Name << "\n";

    // Pack scene files
    const auto result    = IO::ReadBytes("Scenes/Main.xscene");
    auto sceneBytes      = Expect(result, "Failed to read scene bytes");
    auto compressResult  = GZip::Compress(sceneBytes);
    auto compressedBytes = Expect(compressResult, "Failed to compress");

    // Pack script files

    // (MAYBE) Run XPak

    // Compile project code

    // (OPTIONAL) Open output directory in file explorer
}

int main(int argc, char* argv[]) {
    CLI::App app("XEN Engine project compiler and distribution tool.", "XBuild");
    argv = app.ensure_utf8(argv);

    str projectFile;
    app.add_option("-p,--project", projectFile, "Project file path")->required();
    app.add_flag_callback(
      "-v, --version",
      [&]() {
          std::cout << app.get_display_name() << " version 0.0.1-dev" << std::endl;
          std::exit(0);
      },
      "Display app version information.");

    CLI11_PARSE(app, argc, argv);

    XenProject project;
    pugi::xml_document doc;
    if (!doc.load_file(projectFile.c_str())) {
        std::cerr << "Failed to load project" << std::endl;
        return EXIT_FAILURE;
    }
    const auto projectNode   = doc.child("Project");
    project.Name             = projectNode.attribute("name").as_string();
    project.Version          = projectNode.attribute("version").as_string();
    project.Author           = projectNode.attribute("author").as_string();
    project.ContentDirectory = projectNode.child_value("ContentDirectory");
    project.ScenesDirectory  = projectNode.child_value("ScenesDirectory");
    project.ScriptsDirectory = projectNode.child_value("ScriptsDirectory");

    BuildProject(project);

    return 0;
}