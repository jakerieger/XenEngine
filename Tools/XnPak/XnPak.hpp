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
#include <sstream>
#include <algorithm>
#include <fstream>
#include <stb_image.h>
#include <AudioFile.h>

namespace Xen {
    namespace Processors {
        static Vector<u8> processTexture(const str& filename, bool flip = false) {
            i32 width, height, channels;
            stbi_set_flip_vertically_on_load(flip);
            u8* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
            if (!data) {
                stbi_image_free(data);
                Panic("Failed to load image data from file: %s", filename.c_str());
            }

            if (channels != 4) {
                stbi_image_free(data);
                Panic("XnPak only supports 4 channel image formats such as RGBA.");
            }

            Vector<u8> result(width * height * 4);
            memcpy(result.data(), data, width * height * 4);
            stbi_image_free(data);
            return result;
        }

        static Vector<u8> processAudio(const str& filename, bool normalize = false) {
            AudioFile<f32> audioFile;
            if (!audioFile.load(filename)) {
                Panic("Failed to load audio file: %s", filename.c_str());
            }

            if (audioFile.getNumChannels() != 2) {
                Panic("XnPak only supports stereo audio sources.");
            }

            // Interleave audio samples into one vector
            const auto left  = audioFile.samples[0];
            const auto right = audioFile.samples[1];
            std::vector<f32> samples;
            const size_t size = std::min(left.size(), right.size());

            for (size_t i = 0; i < size; ++i) {
                samples.push_back(left[i]);
                samples.push_back(right[i]);
            }

            for (size_t i = size; i < left.size(); ++i) {
                samples.push_back(left[i]);
            }
            for (size_t i = size; i < right.size(); ++i) {
                samples.push_back(right[i]);
            }

            // Normalize samples here to save a step during playback
            if (normalize) {
                const auto maxAbs = std::abs(*std::ranges::max_element(samples, [](f32 a, f32 b) {
                    return std::abs(a) < std::abs(b);
                }));  // Get the highest absolute sample value within our samples array
                if (maxAbs > 0.0f) {
                    f32 scaleFactor = 1.f / maxAbs;
                    for (auto& sample : samples) {
                        sample *= scaleFactor;
                    }
                }
            }

            // Convert sample data to byte array
            Vector<u8> result(samples.size() * sizeof(f32));
            std::memcpy(result.data(), samples.data(), samples.size() * sizeof(f32));

            return result;
        }

        static Vector<u8> processPlainText(const str& filename) {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) { Panic("Failed to open plain text file: %s", filename.c_str()); }

            file.seekg(0, std::ios::end);
            const std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            Vector<u8> result(size);
            if (!file.read(reinterpret_cast<char*>(result.data()), size)) {
                Panic("Failed to read plain text file: %s", filename.c_str());
            }

            file.close();

            return result;
        }

        // Maybe in the future this can generate a sprite font or something, but for now
        // it'll just read the TTF file directly as a byte array and return that.
        static Vector<u8> processFont(const str& filename) {
            return processPlainText(filename);
        }
    }  // namespace Processors

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

        void build() const {
            logStatus("- Building manifest.");
            const auto contentDir = createContentDir();

            std::cout << "  | Created output directory: " << contentDir << '\n';
            std::cout << "  | Building " << assets.size() << " assets.\n";
            buildAssets(contentDir);
        }

        void clean() const {
            logStatus("- Cleaning build directory.");
            const auto contentDir = createContentDir();
            logStatus("- Cleaned build directory.");
        }

        [[nodiscard]] str toString() const {
            std::stringstream ss;
            ss << "OutputDir: " << outputDir;
            ss << "\nCompress: " << compress;
            ss << "\nAssets: " << assets.size();
            return ss.str();
        }

    private:
        [[nodiscard]] Path createContentDir() const {
            // Get content output directory and create it.
            // Relative paths are relative to the location of the manifest file.
            const auto contentDir = Path(rootDir) / outputDir;
            if (FileSystem::exists(contentDir)) { FileSystem::remove_all(contentDir); }
            FileSystem::create_directory(contentDir);
            return contentDir;
        }

        void buildAssets(const Path& contentDir) const {
            const auto assetCount = assets.size();
            auto assetId          = 1;
            for (auto& asset : assets) {
                std::cout << "  | [" << assetId << "/" << assetCount
                          << "] Building asset: " << asset.name << '\n';

                auto split = splitPath(asset.name);
                // Name contains subdirectories, create them
                if (split.size() > 1) {
                    auto currentPath = contentDir;
                    for (int i = 0; i < split.size() - 1; i++) {
                        currentPath /= split[i];
                        if (!FileSystem::exists(currentPath)) {
                            FileSystem::create_directory(currentPath);
                        }
                    }
                }
                Path outputPath = contentDir / asset.name;
                outputPath += ".xpak";

                Vector<u8> data;
                auto srcPath = Path(rootDir) / asset.build;
                switch (asset.type) {
                    case AssetType::Texture:
                        // Process texture
                        {
                            auto bytes = Processors::processTexture(srcPath, true);
                            data.resize(bytes.size());
                            std::memcpy(data.data(), bytes.data(), bytes.size());
                        }
                        break;
                    case AssetType::Audio:
                        // Process audio
                        {
                            auto bytes = Processors::processAudio(srcPath, true);
                            data.resize(bytes.size());
                            std::memcpy(data.data(), bytes.data(), bytes.size());
                        }
                        break;
                    case AssetType::Font:
                        // Process font
                        {
                            auto bytes = Processors::processFont(srcPath);
                            data.resize(bytes.size());
                            std::memcpy(data.data(), bytes.data(), bytes.size());
                        }
                        break;
                    case AssetType::PlainText:
                        // Process plain text (which is just read straight from file)
                        {
                            auto bytes = Processors::processPlainText(srcPath);
                            data.resize(bytes.size());
                            std::memcpy(data.data(), bytes.data(), bytes.size());
                        }
                        break;
                }

                // Write bytes to file
                std::ofstream file(outputPath, std::ios::binary);
                if (!file.is_open()) { Panic("Failed to open output file."); }
                file.write(reinterpret_cast<char*>(data.data()), (std::streamsize)data.size());
                file.close();

                assetId++;
            }
        }

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
