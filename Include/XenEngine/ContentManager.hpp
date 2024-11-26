// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <filesystem>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>
#include <Types.hpp>

#define MAX_ASSET_SIZE 1e10

namespace Xen {
    class Asset {
    public:
        str Name;
        std::vector<u8> Data;
        std::unordered_map<str, str> Metadata;

        Asset() = default;
        Asset(str name, const std::vector<u8>& data, const std::unordered_map<str, str>& metadata)
            : Name(std::move(name)), Data(data), Metadata(metadata) {}
    };

    // TODO: This class currently implements a 'lazy' method of asset loading.
    // Ideally, in a real game you would want to load everything you need for the scene in advance,
    // but for small projects this will work for now.
    class ContentManager {
    public:
        explicit ContentManager(const std::filesystem::path& contentRoot) {
            this->mContentRoot = contentRoot;
        }

        std::optional<Shared<Asset>> LoadAsset(const str& name);

    private:
        std::unordered_map<str, Shared<Asset>> mLoadedAssets;
        std::filesystem::path mContentRoot;

        static bool ValidatePakHeader(const std::vector<u8>& pakBytes);
        static bool ReadMetadata(const std::filesystem::path& filename,
                                 std::unordered_map<str, str>& metadata);
    };
}  // namespace Xen