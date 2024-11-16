// Author: Jake Rieger
// Created: 11/14/2024.
//

#pragma once

#define INC_PAIR
#include "Asset.hpp"
#include "BuildCache.hpp"
#include "Processors.inl"

#include <future>
#include <ranges>
#include <thread>

class Manifest {
public:
    Path RootDir;
    Path OutputDir;
    bool Compress;
    Vector<Asset> Assets;

    explicit Manifest(const str& filename) : Compress(false) {
        Assets.clear();
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result) { Panic("Failed to load manifest file"); }
        RootDir                     = canonical(Path(filename)).parent_path();
        const auto& rootNode        = doc.child("PakManifest");
        OutputDir                   = Path(rootNode.child_value("OutputDir"));
        Compress                    = rootNode.child_value("Compress") == "true";
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
        if (exists(cacheFile)) { buildCache = BuildCache(cacheFile.string().c_str()); }
    }

    void Build() {
        const auto outputDir = CreateOutputDir();
        Vector<Pair<const Asset*, Path>> assetsToBuild;
        for (const auto& asset : Assets) {
            Path sourceFile = RootDir / asset.Source;
            bool rebuild    = false;
            if (auto checksum = buildCache.GetChecksum(asset.Source)) {
                auto currentHash = BuildCache::CalculateChecksum(canonical(sourceFile).string());
                if (currentHash != checksum) {
                    buildCache.Update(asset.Source, currentHash);
                    rebuild = true;
                }
            } else {
                auto currentHash = BuildCache::CalculateChecksum(canonical(sourceFile).string());
                buildCache.Update(asset.Source, currentHash);
                rebuild = true;
            }

            if (rebuild) {
                assetsToBuild.emplace_back(&asset, sourceFile);
            } else {
                std::cout << "  | Skipping unchanged asset: " << asset.Name << '\n';
            }
        }

        Vector<std::future<void>> futures;
        for (const auto& asset : assetsToBuild | std::views::keys) {
            futures.emplace_back(std::async(std::launch::async, [&]() {
                static std::atomic<i32> assetId = 1;
                const int localId               = assetId.fetch_add(1);
                std::cout << "  | [" << localId << "/" << assetsToBuild.size()
                          << "] Building asset: " << asset->Name << '\n';
                BuildAsset(outputDir, *asset, Path(asset->Source));
            }));
        }

        // Wait for all the tasks to complete
        for (auto& future : futures) {
            if (future.valid()) future.get();
        }

        buildCache.SaveToFile(RootDir.string());
    }

    void Rebuild() {
        Clean();
        Build();
    }

    void Clean() {
        const auto _         = CreateOutputDir();
        const auto cacheFile = RootDir / ".build_cache";
        if (exists(cacheFile)) { remove(cacheFile); }
        buildCache.Assets.clear();
    }

private:
    Path manifestPath;
    BuildCache buildCache;

    void CleanContentDirectory() const {
        if (FileSystem::exists(OutputDir)) { FileSystem::remove_all(OutputDir); }
    }

    [[nodiscard]] Path CreateOutputDir() const {
        const auto contentDir = RootDir / OutputDir;
        CleanContentDirectory();
        FileSystem::create_directories(contentDir);
        return contentDir;
    }

    static void BuildAsset(const Path& outputDir, const Asset& asset, const Path& sourceFile) {
        auto outputFile = outputDir / sourceFile;
        outputFile.replace_extension(".xpak");
        if (FileSystem::exists(outputFile)) { FileSystem::remove(outputFile); }
        FileSystem::create_directories(outputFile.parent_path());

        Vector<u8> data;
        switch (asset.Type) {
            case AssetType::Texture:
                data = Processors::ProcessTexture(sourceFile);
                break;
            case AssetType::Audio:
                data = Processors::ProcessAudio(sourceFile);
                break;
            case AssetType::Data:
                data = Processors::ProcessData(sourceFile);
                break;
        }

        if (data.empty()) { return; }

        std::ofstream outFile(outputFile, std::ios::binary);
        outFile.write(reinterpret_cast<char*>(data.data()), (std::streamsize)data.size());
    }
};