// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include "TableEntry.hpp"

#include <IO.hpp>
#include <Expect.hpp>
#include <Panic.hpp>
#include <Types.hpp>
#include <Compression.hpp>
#include <numeric>

class SceneManifest {
public:
    explicit SceneManifest(const Path& filename, const Path& buildDir) : mBuildDir(buildDir) {
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
        const auto sceneCount = mScenes.size();

        size_t sceneDataSize = 0;
        sceneDataSize += sizeof(size_t);                   // For scene count value
        sceneDataSize += sizeof(TableEntry) * sceneCount;  // For scene table
        const size_t headerSize = sceneDataSize;
        // For scene data
        sceneDataSize += std::accumulate(
          mScenes.begin(),
          mScenes.end(),
          0.0,
          [](size_t sum, const std::pair<str, str>& entry) { return sum + entry.second.size(); });

        // Allocate our array once we know the total size
        std::vector<u8> sceneData(sceneDataSize);
        std::vector<TableEntry> sceneEntries;
        size_t offset = headerSize;
        for (const auto& [name, source] : mScenes) {
            sceneEntries.emplace_back(name.c_str(), source.size(), offset);
            offset += source.size();
        }

        // Start copying data to our scene bytes
        memcpy(sceneData.data(), &sceneCount, sizeof(size_t));
        offset = sizeof(size_t);
        for (const auto& entry : sceneEntries) {
            memcpy(sceneData.data() + offset, &entry.Name, strlen(entry.Name));
            offset += 64;
            memcpy(sceneData.data() + offset, &entry.Offset, 4);
            offset += 4;
            memcpy(sceneData.data() + offset, &entry.Size, 4);
            offset += 4;
        }

        // Copy actual scene data
        for (const auto& source : mScenes | std::views::values) {
            memcpy(sceneData.data() + offset, source.c_str(), source.size());
            offset += source.size();
        }

        // Compress our scene data
        const auto result        = GZip::Compress(sceneData);
        const auto compresedData = Expect(result, "Failed to compress scene data");

        const auto filename = mBuildDir / "scene_data.xpkf";
        if (!IO::WriteBytes(filename, compresedData)) { Panic("Failed to write scene data"); }
    }

    void Clean() const {
        const auto filename = mBuildDir / "scene_data.xpkf";
        if (exists(filename)) { remove(filename); }
    }

private:
    Path mBuildDir;
    std::unordered_map<str, str> mScenes;
    Path mSourceDir;
};