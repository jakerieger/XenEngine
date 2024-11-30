// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include "AssetManifest.hpp"
#include "SceneManifest.hpp"
#include "ScriptManifest.hpp"

#include <Panic.hpp>
#include <Types.hpp>
#include <sstream>
#include <pugixml.hpp>

class Project {
public:
    Project() = default;
    str Name;
    str Version;
    str Author;
    Path RootDirectory;
    Path ContentDirectory;
    Path BuildDirectory;
    Unique<AssetManifest> Assets;
    Unique<SceneManifest> Scenes;
    Unique<ScriptManifest> Scripts;

    [[nodiscard]] str ToString() const {
        std::ostringstream os;
        os << Name << '\n';
        os << Version << '\n';
        os << Author << '\n';
        os << ContentDirectory.string() << '\n';
        os << BuildDirectory.string() << '\n';
        return os.str();
    }

    static Unique<Project> Load(const Path& filename) {
        if (!exists(filename)) { Panic("Project file not found"); }
        auto project = std::make_unique<Project>();

        // Parse project file
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result) { Panic("Project file could not be parsed."); }

        const auto root  = doc.child("Project");
        project->Name    = root.attribute("name").as_string();
        project->Version = root.attribute("version").as_string();
        project->Author  = root.attribute("author").as_string();

        const Path contentDir     = root.child_value("ContentDirectory");
        const Path buildDir       = root.child_value("BuildDirectory");
        project->ContentDirectory = canonical(contentDir);
        project->RootDirectory    = project->ContentDirectory.parent_path();
        project->BuildDirectory   = project->RootDirectory / buildDir;

        // Parse manifest files
        const auto assetsManifestFile = project->RootDirectory / "Assets.xml";
        const auto sceneManifestFile  = project->RootDirectory / "Scenes.xml";
        const auto scriptManifestFile = project->RootDirectory / "Scripts.xml";

        if (!exists(assetsManifestFile) or !exists(sceneManifestFile) or
            !exists(scriptManifestFile)) {
            Panic("One or more manifest files not found");
        }

        project->Assets  = std::make_unique<AssetManifest>(assetsManifestFile);
        project->Scenes  = std::make_unique<SceneManifest>(sceneManifestFile);
        project->Scripts = std::make_unique<ScriptManifest>(scriptManifestFile);

        // TODO: Yes these error message suck, I'll fix it later (probably not but who knows)
        if (!project->Assets || !project->Scenes || !project->Scripts) {
            Panic("One or more assets or scenes not found");
        }

        return std::move(project);
    }

    void Build() const {
        Assets->Build();
        Scenes->Build();
        Scripts->Build();
    }

    void Rebuild() const {
        Assets->Rebuild();
        Scenes->Rebuild();
        Scripts->Rebuild();
    }

    void Clean() const {
        Assets->Clean();
        Scenes->Clean();
        Scripts->Clean();
    }
};