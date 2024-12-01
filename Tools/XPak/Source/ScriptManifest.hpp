// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include <Types.hpp>

class ScriptManifest {
public:
    explicit
    ScriptManifest(const Path& filename, const Path& buildDir, const Weak<BuildCache>& buildCache)
        : mBuildDir(buildDir), mBuildCache(buildCache) {
        mSourceDir = filename.parent_path() / "Scripts";
        pugi::xml_document doc;
        const auto result = doc.load_file(filename.string().c_str());
        if (!result) { Panic("Failed to load scene manifest"); }
        const auto root = doc.child("Scripts");
        for (const auto& node : root.children("Script")) {
            const auto script     = node.text().as_string();
            const auto scriptFile = mSourceDir / script;
            if (!exists(scriptFile)) {
                Panic("Failed to find script file: %s", scriptFile.string().c_str());
            }
            const str scriptSource = IO::ReadString(scriptFile);
            mScripts.insert_or_assign(script, scriptSource);
        }
    }

    void Build() {
        const auto cache     = mBuildCache.lock();
        const auto scriptDir = mBuildDir / "scripts";
        if (!exists(scriptDir)) { create_directories(scriptDir); }

        std::vector<std::pair<const str&, const str&>> scriptsToBuild;
        for (const auto& [name, text] : mScripts) {
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
                scriptsToBuild.emplace_back(name, text);
            } else {
                std::cout << "  | Skipping script: " << name << "\n";
            }
        }

        int scriptId = 1;
        for (const auto& [name, text] : scriptsToBuild) {
            std::cout << "  | [" << scriptId++ << "/" << scriptsToBuild.size()
                      << "] Building script: " << name << '\n';
            std::vector<u8> data(text.begin(), text.end());
            const auto result = GZip::Compress(data);
            auto compressed   = Expect(result, "Failed to compress script data");
            auto filename     = scriptDir / name;
            filename.replace_extension(".xbvf");
            std::ofstream outFile(filename, std::ios::binary);
            outFile.write(RCAST<char*>(compressed.data()), (std::streamsize)compressed.size());
            outFile.close();
        }
    }

    void Clean() const {
        const auto scriptsDir = mBuildDir / "scripts";
        if (exists(scriptsDir)) { remove_all(scriptsDir); }
    }

private:
    Path mBuildDir;
    Weak<BuildCache> mBuildCache;
    std::unordered_map<str, str> mScripts;
    Path mSourceDir;
};