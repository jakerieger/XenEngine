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
    explicit Project(const Path& filename) {
        if (!exists(filename)) { Panic("Project file not found"); }

        // Parse project file
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (not result) { Panic("Project file could not be parsed."); }

        const auto root = doc.child("Project");
        mName           = root.attribute("name").as_string();
        mVersion        = root.attribute("version").as_string();
        mAuthor         = root.attribute("author").as_string();

        const Path contentDir = root.child_value("ContentDirectory");
        const Path buildDir   = root.child_value("BuildDirectory");
        mContentDir           = canonical(contentDir);
        mRootDir              = mContentDir.parent_path();
        mBuildDir             = mRootDir / buildDir;

        // Parse manifest files
        const auto assetsManifestFile = mContentDir / "Assets.xml";
        const auto sceneManifestFile  = mContentDir / "Scenes.xml";
        const auto scriptManifestFile = mContentDir / "Scripts.xml";

        if (!exists(assetsManifestFile) or !exists(sceneManifestFile) or
            !exists(scriptManifestFile)) {
            Panic("One or more manifest files not found");
        }

        const auto cacheFile = mRootDir / ".build_cache";
        if (exists(cacheFile)) {
            mBuildCache = std::make_shared<BuildCache>(cacheFile.string().c_str());
        } else {
            mBuildCache = std::make_shared<BuildCache>();
        }

        mAssets  = std::make_unique<AssetManifest>(assetsManifestFile, mBuildDir, mBuildCache);
        mScenes  = std::make_unique<SceneManifest>(sceneManifestFile, mBuildDir, mBuildCache);
        mScripts = std::make_unique<ScriptManifest>(scriptManifestFile, mBuildDir, mBuildCache);

        // TODO: Yes these error message suck, I'll fix it later (probably not but who knows)
        if (!mAssets || !mScenes || !mScripts) { Panic("One or more assets or scenes not found"); }
    }

    [[nodiscard]] str ToString() const {
        std::ostringstream os;
        os << mName << '\n';
        os << mVersion << '\n';
        os << mAuthor << '\n';
        os << mContentDir.string() << '\n';
        os << mBuildDir.string() << '\n';
        return os.str();
    }

    void Build() const {
        // Create the build directory
        if (!exists(mBuildDir)) {
            if (!create_directory(mBuildDir)) { Panic("Failed to create build directory"); }
        }

        mAssets->Build();
        mScenes->Build();
        mScripts->Build();

        mBuildCache->SaveToFile(mRootDir.string());
    }

    void Rebuild() const {
        Clean();
        Build();
    }

    void Clean() const {
        mAssets->Clean();
        mScenes->Clean();
        mScripts->Clean();
        mBuildCache->Clear();
        const auto cacheFile = mRootDir / ".build_cache";
        if (exists(cacheFile)) { remove(cacheFile); }
    }

private:
    str mName;
    str mVersion;
    str mAuthor;
    Path mRootDir;
    Path mContentDir;
    Path mBuildDir;
    Unique<AssetManifest> mAssets;
    Unique<SceneManifest> mScenes;
    Unique<ScriptManifest> mScripts;
    Shared<BuildCache> mBuildCache;
};