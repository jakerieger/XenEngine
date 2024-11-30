// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include <Types.hpp>

class ScriptManifest {
public:
    explicit
    ScriptManifest(const Path& filename, const Path& buildDir, const Weak<BuildCache>& buildCache)
        : mBuildDir(buildDir), mBuildCache(buildCache) {}

    void Build() {}
    void Rebuild() {}
    void Clean() {}

private:
    Path mBuildDir;
    Weak<BuildCache> mBuildCache;
};