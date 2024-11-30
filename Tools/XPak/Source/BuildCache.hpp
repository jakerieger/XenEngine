// Author: Jake Rieger
// Created: 11/14/2024.
//

// ReSharper disable CppDeprecatedEntity
#pragma once
#pragma warning(disable : 4996)

#include <Panic.hpp>
#include <pugixml.hpp>
#include <fstream>
#include <optional>
#include <sha256.h>
#include <filesystem>

#include <Types.hpp>
#include <unordered_map>
#include <vector>

class BuildCache {
public:
    BuildCache() = default;

    explicit BuildCache(const char* filename) {
        this->mFiles.clear();
        pugi::xml_document doc;
        const auto parseResult = doc.load_file(filename);
        if (!parseResult) { Panic("Failed to parse XML file"); }

        const auto cacheRoot = doc.child("BuildCache");
        for (auto& asset : cacheRoot.children("Asset")) {
            const auto& source   = asset.attribute("source").value();
            const auto& checksum = asset.text().as_string();
            this->mFiles.insert_or_assign(source, checksum);
        }
    }

    void SaveToFile(const str& rootDir) {
        pugi::xml_document doc;
        auto rootNode = doc.append_child("BuildCache");
        for (const auto& [source, checksum] : this->mFiles) {
            auto assetNode  = rootNode.append_child("Asset");
            auto sourceAttr = assetNode.append_attribute("source");
            sourceAttr.set_value(source.c_str());
            assetNode.text().set(checksum.c_str());
        }
        const auto outFile = std::filesystem::path(rootDir) / ".build_cache";
        if (!doc.save_file(outFile.string().c_str())) { Panic("Failed to save build cache"); }
    }

    std::optional<str> GetChecksum(const str& key) {
        const auto it = mFiles.find(key);
        if (it != mFiles.end()) return it->second;
        return std::nullopt;
    }

    [[nodiscard]] static str CalculateChecksum(const str& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) { Panic("Failed to open file"); }

        SHA256 sha256;
        std::vector<char> buffer(4096);
        while (file.read(buffer.data(), CAST<std::streamsize>(buffer.size())) ||
               file.gcount() > 0) {
            // Only update SHA256 if there's data read
            sha256.add(buffer.data(), file.gcount());
        }

        u8 hash[32];
        sha256.getHash(hash);

        std::ostringstream result;
        result << std::hex << std::setfill('0');
        for (const u8 byte : hash) {
            result << std::setw(2) << CAST<int>(byte);
        }

        return result.str();
    }

    void Update(const str& key, const str& value) {
        this->mFiles.insert_or_assign(key, value);
    }

    void Clear() {
        this->mFiles.clear();
    }

private:
    std::unordered_map<str, str> mFiles;
};
