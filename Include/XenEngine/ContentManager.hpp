// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <Types.hpp>

#define MAX_ASSET_SIZE 1e10

namespace Xen {
    class Asset {
    public:
        str Name;
        std::vector<u8> Data;

        Asset() = default;
        Asset(const str& name, const std::vector<u8>& data) : Name(name), Data(data) {}
    };

    // TODO: This class currently implements a 'lazy' method of asset loading.
    // Ideally, in a real game you would want to load everything you need for the scene in advance,
    // but for small projects this will work for now.
    class ContentManager {
    public:
        explicit ContentManager(const std::filesystem::path& contentRoot) {
            this->mContentRoot = contentRoot;
        }

        std::optional<Asset> LoadAsset(const str& name);

    private:
        std::unordered_map<str, Asset> mLoadedAssets;
        std::filesystem::path mContentRoot;

        static bool ValidatePakHeader(const std::vector<u8>& pakBytes);
    };
}  // namespace Xen