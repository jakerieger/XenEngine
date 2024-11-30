// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include <Types.hpp>

class ScriptManifest {
public:
    Path RootDir;

    explicit ScriptManifest(const Path& filename, const Path& root) : RootDir(root) {}

    void Build() {}
    void Rebuild() {}
    void Clean() {}
};