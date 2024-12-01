// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include "Expect.hpp"
#include "IO.hpp"

#include <Panic.hpp>
#include <Types.hpp>
#include <Compression.hpp>
#include <pugixml.hpp>

class SceneManifest {
public:
    explicit
    SceneManifest(const Path& filename, const Path& buildDir, const Weak<BuildCache>& buildCache)
        : mBuildDir(buildDir), mBuildCache(buildCache) {
        mSourceDir = filename.parent_path() / "Scenes";
        pugi::xml_document doc;
        const auto result = doc.load_file(filename.string().c_str());
        if (!result) { Panic("Failed to load scene manifest"); }
        const auto root = doc.child("Scenes");
        for (const auto& node : root.children("Scene")) {
            const auto scene     = node.text().as_string();
            const auto sceneFile = mSourceDir / scene;
            if (!exists(sceneFile)) { Panic("Failed to find scene file"); }
            const str sceneText = IO::ReadString(sceneFile);
            mScenes.insert_or_assign(scene, sceneText);
        }
    }

    void Build() {
        const auto cache    = mBuildCache.lock();
        const auto sceneDir = mBuildDir / "scenes";
        if (!exists(sceneDir)) { create_directories(sceneDir); }

        std::vector<std::pair<const str&, const str&>> scenesToBuild;
        for (const auto& [name, text] : mScenes) {
            auto sourceFile = mSourceDir / name;
            bool rebuild    = false;
            if (auto checksum = cache->GetChecksum(name)) {
                auto currentHash = BuildCache::CalculateChecksum(sourceFile.string());
                if (currentHash != checksum) {
                    cache->Update(name, currentHash);
                    rebuild = true;
                }
            } else {
                auto currentHash = BuildCache::CalculateChecksum(sourceFile.string());
                cache->Update(name, currentHash);
                rebuild = true;
            }

            if (rebuild) {
                scenesToBuild.emplace_back(name, text);
            } else {
                std::cout << "  | Skipping scene: " << name << "\n";
            }
        }

        int sceneId = 1;
        for (const auto& [name, text] : scenesToBuild) {
            std::cout << "  | [" << sceneId++ << "/" << scenesToBuild.size()
                      << "] Building scene: " << name << '\n';
            std::vector<u8> data(text.begin(), text.end());
            const auto result = GZip::Compress(data);
            auto compressed   = Expect(result, "Failed to compress scene data");
            auto filename     = sceneDir / name;
            filename.replace_extension(".xscf");
            std::ofstream outFile(filename, std::ios::binary);
            outFile.write(RCAST<char*>(compressed.data()), (std::streamsize)compressed.size());
            outFile.close();
        }
    }

    void Clean() const {
        const auto scenesDir = mBuildDir / "scenes";
        if (exists(scenesDir)) { remove_all(scenesDir); }
    }

private:
    Path mBuildDir;
    Weak<BuildCache> mBuildCache;
    std::unordered_map<str, str> mScenes;
    Path mSourceDir;
};