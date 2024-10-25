// Author: Jake Rieger
// Created: 10/23/2024.
//

#pragma once

#define INC_VECTOR
#include <Types/STL.h>
#include <Types/Types.h>
#include <Types/IO.h>
#include <Panic.h>
#include <cstring>

#include <pugixml.hpp>
#include <ranges>
#include <sstream>
#include <algorithm>

namespace Xen {
    enum class AssetType : u8 {
        Texture,
        Audio,
        Font,
        PlainText,
    };

    class Asset {
    public:
        str name;
        AssetType type;
        str build;

        Asset(const str& name, const str& type, const str& build) {
            this->name  = name;
            this->type  = getType(type);
            this->build = build;
        }

        [[nodiscard]] str toString() const {
            std::stringstream ss;
            ss << "Name: " << name;
            ss << ", Type: " << (u32)type;
            ss << ", Build: " << build;
            return ss.str();
        }

    private:
        [[nodiscard]] static AssetType getType(const str& type) {
            if (type == "Texture") {
                return AssetType::Texture;
            } else if (type == "Audio") {
                return AssetType::Audio;
            } else if (type == "Font") {
                return AssetType::Font;
            } else if (type == "PlainText") {
                return AssetType::PlainText;
            }

            return AssetType::PlainText;
        }
    };

    class Manifest {
    public:
        str rootDir;
        str outputDir;
        bool compress;
        Vector<Asset> assets;

        explicit Manifest(const str& filename) {
            rootDir = FileSystem::absolute(filename).parent_path().string();

            pugi::xml_document doc;
            const auto result = doc.load_file(filename.c_str());
            if (!result) { Panic("Failed to parse manifest."); }
            if (!validateManifest(doc)) { Panic("Invalid manifest."); }

            const auto root = doc.root().first_child();

            outputDir             = root.child("OutputDir").text().as_string();
            compress              = strcmp(root.child("Compress").text().as_string(), "true") == 0;
            const auto assetNodes = root.child("Content").children("Asset");
            for (const auto& asset : assetNodes) {
                assets.emplace_back(asset.attribute("name").value(),
                                    asset.child("Type").text().as_string(),
                                    asset.child("Build").text().as_string());
            }
        }

        void build() {}

        void clean() {}

        [[nodiscard]] str toString() const {
            std::stringstream ss;
            ss << "OutputDir: " << outputDir;
            ss << "\nCompress: " << compress;
            ss << "\nAssets: " << assets.size();
            return ss.str();
        }

    private:
        static bool validateAsset(const pugi::xml_node& node) {
            if (strcmp(node.name(), "Asset") != 0) return false;
            if (node.attribute("name").empty()) return false;
            if (node.child("Type").empty()) return false;
            if (node.child("Build").empty()) return false;
            return true;
        }

        static bool validateManifest(const pugi::xml_document& manifest) {
            if (manifest.root().empty()) return false;
            if (manifest.root().first_child().empty()) return false;
            const auto root = manifest.root().first_child();
            if (root.child("OutputDir").empty()) return false;
            if (root.child("Compress").empty()) return false;
            if (root.child("Content").empty()) return false;
            if (root.child("Content").children("Asset").empty()) return false;

            const auto assets = root.child("Content").children("Asset");
            return std::ranges::all_of(assets, [](const pugi::xml_node& asset) {
                return validateAsset(asset);
            });
        }
    };
}  // namespace Xen
