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

namespace fs = std::filesystem;

class Manifest {
public:
    fs::path RootDir;
    fs::path OutputDir;
    bool Compress;
    std::vector<Asset> Assets;

    explicit Manifest(const str& filename) : Compress(false) {
        Assets.clear();
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result) { Panic("Failed to load manifest file"); }
        RootDir                     = fs::canonical(filename).parent_path();
        const auto& rootNode        = doc.child("PakManifest");
        OutputDir                   = fs::path(rootNode.child_value("OutputDir"));
        Compress                    = rootNode.child("Compress").text().as_bool();
        const auto& contentNode     = rootNode.child("Content");
        const auto& contentChildren = contentNode.children("Asset");
        const i64 assetCount        = std::distance(contentChildren.begin(), contentChildren.end());
        Assets.reserve(assetCount);
        for (const auto& asset : contentChildren) {
            const auto& assetName   = asset.attribute("name").as_string();
            const auto& assetType   = asset.child_value("Type");
            const auto& assetSource = asset.child_value("Source");
            Assets.emplace_back(assetName, assetType, assetSource);
        }

        // Load build cache if it exists
        const auto cacheFile = RootDir / ".build_cache";
        if (exists(cacheFile)) {
            buildCache = std::make_unique<BuildCache>(cacheFile.string().c_str());
        } else {
            buildCache = std::make_unique<BuildCache>();
        }
    }

    ~Manifest() {
        buildCache.reset();
    }

    void Build() const {
        const auto outputDir = CreateOutputDir();
        std::vector<std::pair<const Asset*, fs::path>> assetsToBuild;
        for (const auto& asset : Assets) {
            fs::path sourceFile = RootDir / asset.Source;
            bool rebuild        = false;
            if (auto checksum = buildCache->GetChecksum(asset.Source)) {
                auto currentHash = BuildCache::CalculateChecksum(canonical(sourceFile).string());
                if (currentHash != checksum) {
                    buildCache->Update(asset.Source, currentHash);
                    rebuild = true;
                }
            } else {
                auto currentHash = BuildCache::CalculateChecksum(canonical(sourceFile).string());
                buildCache->Update(asset.Source, currentHash);
                rebuild = true;
            }

            if (rebuild) {
                assetsToBuild.emplace_back(&asset, sourceFile);
            } else {
                std::cout << "  | Skipping unchanged asset: " << asset.Name << '\n';
            }
        }

        std::vector<std::future<void>> futures;
        for (const auto& asset : assetsToBuild | std::views::keys) {
            futures.emplace_back(std::async(std::launch::async, [&]() {
                static std::atomic<i32> assetId = 1;
                const int localId               = assetId.fetch_add(1);
                std::cout << "  | [" << localId << "/" << assetsToBuild.size()
                          << "] Building asset: " << asset->Name << '\n';
                BuildAsset(outputDir, *asset, std::filesystem::path(asset->Source), Compress);
            }));
        }

        // Wait for all the tasks to complete
        for (auto& future : futures) {
            if (future.valid()) future.get();
        }

        buildCache->SaveToFile(RootDir.string());
    }

    void Rebuild() const {
        Clean();
        Build();
    }

    void Clean() const {
        const auto _         = CreateOutputDir();
        const auto cacheFile = RootDir / ".build_cache";
        if (exists(cacheFile)) { remove(cacheFile); }
        buildCache->Assets.clear();
    }

private:
    fs::path manifestPath;
    Unique<BuildCache> buildCache;

    void CleanContentDirectory() const {
        if (fs::exists(OutputDir)) { fs::remove_all(OutputDir); }
    }

    [[nodiscard]] fs::path CreateOutputDir() const {
        const auto contentDir = RootDir / OutputDir;
        CleanContentDirectory();
        fs::create_directories(contentDir);
        return contentDir;
    }

    static void BuildAsset(const fs::path& outputDir,
                           const Asset& asset,
                           const fs::path& sourceFile,
                           const bool compress = false) {
        auto outputFile = outputDir / sourceFile;
        outputFile.replace_extension(".xpkf");
        if (fs::exists(outputFile)) { fs::remove(outputFile); }
        fs::create_directories(outputFile.parent_path());

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
        if (compress) {
            std::cout << "  |  -  Compressing...\n";
            const auto result = Compression::Compress(data);
            if (result.has_value()) {
                const auto& compressedData = result.value();
                data.assign(compressedData.begin(), compressedData.end());
                data.shrink_to_fit();  // probably not necessary
            }
        }

        if (!PakFile::Write(data, outputFile, compress, originalSize)) {
            std::cout << "  |  [ERROR] Writing to Pak file failed.\n";
        }

        const auto metadataFile = outputFile.replace_extension(".xmdf");
        if (!MetadataFile::Write(metadataFile, metadata)) {
            std::cout << "  |  [ERROR] Writing to metadata file failed.\n";
        }
    }
};