// Author: Jake Rieger
// Created: 11/14/2024.
//

#pragma once

#include <Types.hpp>
#include <unordered_map>

enum class AssetType {
    Texture,
    Audio,
    Data,
};

static AssetType GetAssetTypeFromString(const str& assetType) {
    if (assetType == "Texture") {
        return AssetType::Texture;
    } else if (assetType == "Audio") {
        return AssetType::Audio;
    } else {
        return AssetType::Data;
    }
}

static str AssetTypeToString(const AssetType& type) {
    switch (type) {
        case AssetType::Texture:
            return "Texture";
        case AssetType::Audio:
            return "Audio";
        case AssetType::Data:
        default:
            return "Data";
    }
}

struct Asset {
    str Name;
    AssetType Type;
    str Source;

    Asset(str name, const str& type, str source) {
        this->Name   = std::move(name);
        this->Type   = GetAssetTypeFromString(type);
        this->Source = std::move(source);
    }
};