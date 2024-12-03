// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include <Types.hpp>
#include <IO.hpp>
#include <Expect.hpp>

#include "TableEntry.hpp"

class ScriptManifest {
public:
    explicit ScriptManifest(const Path& filename, const Path& buildDir) : mBuildDir(buildDir) {
        mSourceDir = filename.parent_path() / "Scripts";
        pugi::xml_document doc;
        const auto result = doc.load_file(filename.string().c_str());
        if (!result) { Panic("Failed to load script manifest"); }
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
        const auto scriptCount = mScripts.size();

        size_t scriptDataSize = 0;
        scriptDataSize += sizeof(size_t);                    // For script count value
        scriptDataSize += sizeof(TableEntry) * scriptCount;  // For script table
        const size_t headerSize = scriptDataSize;
        // For script data
        scriptDataSize += std::accumulate(
          mScripts.begin(),
          mScripts.end(),
          0.0,
          [](size_t sum, const std::pair<str, str>& entry) { return sum + entry.second.size(); });

        // Allocate our array once we know the total size
        std::vector<u8> scriptData(scriptDataSize);
        std::vector<TableEntry> scriptEntries;
        size_t offset = headerSize;
        for (const auto& [name, source] : mScripts) {
            scriptEntries.emplace_back(name.c_str(), source.size(), offset);
            offset += source.size();
        }

        // Start copying data to our script bytes
        memcpy(scriptData.data(), &scriptCount, sizeof(size_t));
        offset = sizeof(size_t);
        for (const auto& entry : scriptEntries) {
            memcpy(scriptData.data() + offset, &entry.Name, strlen(entry.Name));
            offset += 64;
            memcpy(scriptData.data() + offset, &entry.Offset, 4);
            offset += 4;
            memcpy(scriptData.data() + offset, &entry.Size, 4);
            offset += 4;
        }

        // Copy actual script data
        for (const auto& source : mScripts | std::views::values) {
            memcpy(scriptData.data() + offset, source.c_str(), source.size());
            offset += source.size();
        }

        // Compress our script data
        const auto result        = GZip::Compress(scriptData);
        const auto compresedData = Expect(result, "Failed to compress script data");

        const auto filename = mBuildDir / "script_data.xpkf";
        if (!IO::WriteBytes(filename, compresedData)) { Panic("Failed to write script data"); }
    }

    void Clean() const {
        const auto filename = mBuildDir / "script_data.xpkf";
        if (exists(filename)) { remove(filename); }
    }

private:
    Path mBuildDir;
    std::unordered_map<str, str> mScripts;
    Path mSourceDir;
};