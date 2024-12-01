// Author: Jake Rieger
// Created: 11/14/2024.
//

#pragma once

#include "Asset.hpp"
#include "BuildCache.hpp"
#include "MetadataFile.hpp"
#include "PakFile.hpp"
#include "Processors.inl"

#include <Types.hpp>
#include <Compression.hpp>
#include <future>
#include <ranges>
#include <thread>
#include <filesystem>
#include <utility>

class AssetManifest {
public:
    explicit
    AssetManifest(const Path& filename, const Path& buildDir, const Weak<BuildCache>& buildCache)
        : mBuildDir(buildDir), mBuildCache(buildCache) {
        mSourceDir = filename.parent_path() / "Assets";
        mAssets.clear();
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result) { Panic("Failed to load manifest file"); }
        const auto& rootNode = doc.child("Assets");
        if (rootNode.empty()) { Panic("Failed to load manifest file"); }
        for (const auto& node : rootNode.children("Asset")) {
            const auto name   = node.attribute("name").as_string();
            const auto type   = node.child_value("Type");
            const auto source = node.child_value("Source");
            mAssets.emplace_back(name, type, source);
        }
    }

    ~AssetManifest() {
        mAssets.clear();
    }

    void Build() const {
        const auto assetDir = mBuildDir / "assets";
        if (!exists(assetDir)) { create_directories(assetDir); }

        std::vector<const Asset*> assetsToBuild;
        const auto cache = mBuildCache.lock();
        for (const auto& asset : mAssets) {
            auto sourceFile = mSourceDir / asset.Source;
            bool rebuild    = false;
            if (auto checksum = cache->GetChecksum(asset.Source)) {
                auto currentHash = BuildCache::CalculateChecksum(sourceFile.string());
                if (currentHash != checksum) {
                    cache->Update(asset.Source, currentHash);
                    rebuild = true;
                }
            } else {
                auto currentHash = BuildCache::CalculateChecksum(sourceFile.string());
                cache->Update(asset.Source, currentHash);
                rebuild = true;
            }

            if (rebuild) {
                assetsToBuild.emplace_back(&asset);
            } else {
                std::cout << "  | Skipping asset: " << asset.Name << "\n";
            }
        }

        int assetId = 1;
        for (const auto& asset : assetsToBuild) {
            std::cout << "  | [" << assetId++ << "/" << assetsToBuild.size()
                      << "] Building asset: " << asset->Name << '\n';
            auto sourceFile = mSourceDir / asset->Source;
            BuildAsset(assetDir, *asset, sourceFile);
        }
    }

    void Clean() const {
        const auto assetDir = mBuildDir / "assets";
        if (exists(assetDir)) { remove_all(assetDir); }
    }

private:
    Path mBuildDir;
    Path mSourceDir;
    std::vector<Asset> mAssets;
    Weak<BuildCache> mBuildCache;

    static void BuildAsset(const Path& outputDir, const Asset& asset, const Path& sourceFile) {
        auto outputFile = outputDir / asset.Name;
        outputFile.replace_extension(".xpkf");
        if (exists(outputFile)) { remove(outputFile); }
        create_directories(outputFile.parent_path());

        std::vector<u8> data;
        std::unordered_map<str, str> metadata;
        switch (asset.Type) {
            case AssetType::Texture:
                data = Processors::ProcessTexture(sourceFile, metadata);
                break;
            case AssetType::Audio:
                data = Processors::ProcessAudio(sourceFile, metadata);
                break;
            case AssetType::Data:
                data = Processors::ProcessData(sourceFile, metadata);
                break;
        }

        if (data.empty()) { return; }

        const auto originalSize = data.size();
        std::cout << "  |  -  Compressing...\n";
        const auto result = LZMA::Compress(data);
        if (result.has_value()) {
            const auto& compressedData = result.value();
            data.assign(compressedData.begin(), compressedData.end());
            data.shrink_to_fit();  // probably not necessary
        }

        if (!PakFile::Write(data, outputFile, compress, originalSize)) {
            std::cout << "  |  [ERROR] Writing to Pak file failed.\n";
        }

        const auto metadataFile = outputFile.replace_extension(".xmdf");
        if (exists(metadataFile)) { remove(metadataFile); }
        if (!MetadataFile::Write(metadataFile, metadata)) {
            std::cout << "  |  [ERROR] Writing to metadata file failed.\n";
        }
    }
};