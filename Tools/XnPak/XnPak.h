// Author: Jake Rieger
// Created: 10/23/2024.
//

#pragma once

#define INC_VECTOR
#include "Platform.h"

#include <Types/STL.h>
#include <Types/Types.h>
#include <Types/IO.h>

#include <pugixml.hpp>
#include <sstream>

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

            auto root = doc.root().first_child();

            outputDir             = root.child("OutputDir").text().as_string();
            compress              = root.child("Compress").text().as_string() == "true";
            const auto assetNodes = root.child("Content").children("Asset");
            for (const auto& asset : assetNodes) {
                assets.emplace_back(asset.attribute("name").value(),
                                    asset.child("Type").text().as_string(),
                                    asset.child("Build").text().as_string());
            }
        }

        [[nodiscard]] str toString() const {
            std::stringstream ss;
            ss << "OutputDir: " << outputDir;
            ss << "\nCompress: " << compress;
            ss << "\nAssets: " << assets.size();
            return ss.str();
        }

    private:
        bool validateAsset() const {
            return true;
        }

        bool validateManifest(const pugi::xml_document& manifest) const {
            return true;
        }
    };
}  // namespace Xen
