// Author: Jake Rieger
// Created: 11/14/2024.
//

// ReSharper disable CppDeprecatedEntity
#pragma once
#pragma warning(disable : 4996)

#include <Panic.hpp>
#include <pugixml.hpp>
#include <openssl/sha.h>
#include <fstream>

#define INC_DICTIONARY
#define INC_VECTOR
#define INC_OPTION
#include <Types/Cast.h>
#include <Types/IO.h>
#include <Types/STL.h>

struct BuildCache {
    Dictionary<str, str> Assets;

    BuildCache() = default;

    explicit BuildCache(const char* filename) {
        this->Assets.clear();
        pugi::xml_document doc;
        const auto parseResult = doc.load_file(filename);
        if (!parseResult) { Panic("Failed to parse XML file"); }

        const auto cacheRoot = doc.child("BuildCache");
        for (auto& asset : cacheRoot.children("Asset")) {
            const auto& source   = asset.attribute("source").value();
            const auto& checksum = asset.value();
            this->Assets.insert_or_assign(source, checksum);
        }
    }

    void SaveToFile(const str& rootDir) {
        pugi::xml_document doc;
        auto rootNode = doc.append_child("BuildCache");
        for (const auto& [source, checksum] : this->Assets) {
            auto assetNode  = rootNode.append_child("Asset");
            auto sourceAttr = assetNode.append_attribute("source");
            sourceAttr.set_value(source.c_str());
            assetNode.set_value(checksum.c_str());
        }
        const auto outFile = Path(rootDir) / ".build_cache";
        if (!doc.save_file(outFile.string().c_str())) { Panic("Failed to save build cache"); }
    }

    Option<str> GetChecksum(const str& key) {
        const auto it = Assets.find(key);
        if (it != Assets.end()) return it->second;
        return std::nullopt;
    }

    [[nodiscard]] static str CalculateChecksum(const str& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) { Panic("Failed to open file"); }

        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        Vector<char> buffer(4096);

        while (file.read(buffer.data(), CAST<std::streamsize>(buffer.size())) ||
               file.gcount() > 0) {
            // Only update SHA256 if there's data read
            SHA256_Update(&sha256, buffer.data(), file.gcount());
        }

        u8 hash[SHA256_DIGEST_LENGTH];
        SHA256_Final(hash, &sha256);

        std::ostringstream result;
        result << std::hex << std::setfill('0');
        for (const u8 byte : hash) {
            result << std::setw(2) << CAST<int>(byte);
        }

        return result.str();
    }

    void Update(const str& key, const str& value) {
        this->Assets.insert_or_assign(key, value);
    }

    void Clear() {
        this->Assets.clear();
    }
};
