// Author: Jake Rieger
// Created: 10/22/2024.
//

#include "ContentManager.hpp"

#include <fstream>
#include <Compression.hpp>
#include <Types/Cast.h>

namespace Xen {
    std::optional<Asset> ContentManager::LoadAsset(const str& name) {
        std::cout << "Loading asset: " << name << '\n';

        const auto it = mLoadedAssets.find(name);
        if (it != mLoadedAssets.end()) { return it->second; }

        auto fileName = mContentRoot / name;
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
        std::vector<u8> pakBytes(fileSize);
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

        std::vector<u8> srcData(pakBytes.size() - 16);
        std::memcpy(srcData.data(), pakBytes.data() + 16, pakBytes.size() - 16);

        std::vector<u8> data(originalSize);
        if (compressed) {
            auto result       = Compression::Decompress(srcData, originalSize);
            auto decompressed = Expect(result, "Failed to decompress asset data");
            memcpy(data.data(), decompressed.data(), decompressed.size());
        } else {
            std::memcpy(data.data(), srcData.data(), originalSize);
        }

        return Asset(name, data);
    }

    bool ContentManager::ValidatePakHeader(const std::vector<u8>& pakBytes) {
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
}  // namespace Xen