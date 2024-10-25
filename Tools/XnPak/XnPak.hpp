// Author: Jake Rieger
// Created: 10/23/2024.
//

#pragma once

#define INC_VECTOR
#include <Types/STL.h>
#include <Types/Types.h>
#include <Types/IO.h>
#include <Panic.hpp>
#include <cstring>

#include <pugixml.hpp>
#include <ranges>
#include <sstream>
#include <algorithm>

namespace Xen {
    static void logStatus(const str& msg) {
        std::cout << msg << '\n';
    }

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

        void build() {
            logStatus("- Building manifest.");

            // Get content output directory and create it.
            // Relative paths are relative to the location of the manifest file.
            const auto contentDir = Path(rootDir) / outputDir;
            if (FileSystem::exists(contentDir)) { FileSystem::remove_all(contentDir); }
            FileSystem::create_directory(contentDir);

            std::cout << "  | Created output directory: " << contentDir << '\n';
            std::cout << "  | Found " << assets.size() << " assets.\n";

            auto assetCount = assets.size();
            auto assetId    = 1;
            for (auto& asset : assets) {
                std::cout << "  | [" << assetId << "/" << assetCount
                          << "] Building asset: " << asset.name << '\n';

                str outputPath;
                auto filename = asset.name;
                auto split    = splitPath(filename);
                // Name contains subdirectories, create them
                if (split.size() > 1) {
                    filename         = split.back();
                    auto currentPath = Path(contentDir);
                    for (int i = 0; i < split.size() - 1; i++) {
                        currentPath /= split[i];
                        if (!FileSystem::exists(currentPath)) {
                            FileSystem::create_directory(currentPath);
                        }
                    }
                }

                assetId++;
            }
        }

        void clean() {}

        [[nodiscard]] str toString() const {
            std::stringstream ss;
            ss << "OutputDir: " << outputDir;
            ss << "\nCompress: " << compress;
            ss << "\nAssets: " << assets.size();
            return ss.str();
        }

    private:
        static Vector<str> splitPath(const str& path) {
            Vector<str> result;
            str current;

            for (const char c : path) {
                if (c == '/' || c == '\\') {
                    if (!current.empty()) {
                        result.push_back(current);
                        current.clear();
                    }
                } else {
                    current += c;
                }
            }

            if (!current.empty()) { result.push_back(current); }

            return result;
        }

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
