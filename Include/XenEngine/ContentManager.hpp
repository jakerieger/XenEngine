// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#define TYPES_ALL
#define INC_VECTOR
#define INC_OPTION
#define INC_DICTIONARY
#include <Types/Types.h>
#include <fstream>
#include <iostream>

#include "../Tools/XPak/Source/Compression.inl"

#define MAX_ASSET_SIZE 1e10

namespace Xen {
    class Asset {
    public:
        str Name;
        Vector<u8> Data;

        Asset() = default;
        Asset(const str& name, const Vector<u8>& data) : Name(name), Data(data) {}
    };

    // TODO: This class currently implements a 'lazy' method of asset loading.
    // Ideally, in a real game you would want to load everything you need for the scene in advance,
    // but for small projects this will work for now.
    class ContentManager {
    public:
        explicit ContentManager(const Path& contentRoot) {
            this->contentRoot = contentRoot;
        }

        Option<Asset> LoadAsset(const str& name) {
            std::cout << "Loading asset: " << name << '\n';

            const auto it = loadedAssets.find(name);
            if (it != loadedAssets.end()) { return it->second; }

            auto fileName = contentRoot / name;
            fileName.replace_extension(".xpak");
            if (!exists(fileName)) {
                std::cout << "Unable to locate asset: " << fileName.string() << std::endl;
                return {};
            }

            std::ifstream file(fileName, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                std::cout << "Unable to open file: " << fileName.string() << std::endl;
                return {};
            }
            const std::streamsize fileSize = file.tellg();
            Vector<u8> pakBytes(fileSize);
            file.seekg(0, std::ios::beg);
            if (!file.read(RCAST<char*>(pakBytes.data()), fileSize)) {
                std::cout << "Unable to read file: " << fileName.string() << std::endl;
                return {};
            }
            file.close();

            if (!ValidatePakHeader(pakBytes)) {
                std::cout << "Invalid PAK file: " << fileName.string() << std::endl;
                return {};
            }

            bool compressed = false;
            std::memcpy(&compressed, pakBytes.data() + 7, 1);

            size_t originalSize;
            std::memcpy(&originalSize, pakBytes.data() + 8, sizeof(size_t));

            Vector<u8> srcData(pakBytes.size() - 16);
            std::memcpy(srcData.data(), pakBytes.data() + 16, pakBytes.size() - 16);

            Vector<u8> data(originalSize);
            if (compressed) {
                auto result       = Compression::Decompress(srcData, originalSize);
                auto decompressed = Expect(result, "Failed to decompress asset data");
                memcpy(data.data(), decompressed.data(), decompressed.size());
            } else {
                std::memcpy(data.data(), srcData.data(), originalSize);
            }

            return Asset(name, data);
        }

    private:
        Dictionary<str, Asset> loadedAssets;
        Path contentRoot;

        static bool ValidatePakHeader(const Vector<u8>& pakBytes) {
            char secret[5] = {'\0'};
            std::memcpy(&secret[0], pakBytes.data(), 4);
            if (strcmp("XPAK", secret) != 0) { return false; }

            if (pakBytes[4] != '\0' || pakBytes[5] != '\0' || pakBytes[6] != '\0') { return false; }

            size_t originalSize;
            std::memcpy(&originalSize, pakBytes.data() + 8, sizeof(size_t));
            if (originalSize > (size_t)MAX_ASSET_SIZE) { return false; }

            if (originalSize < (pakBytes.size() - 16)) { return false; }

            return true;
        }
    };
}  // namespace Xen