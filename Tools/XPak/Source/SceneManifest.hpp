// Author: Jake Rieger
// Created: 11/30/2024.
//

#pragma once

#include <Types.hpp>

class SceneManifest {
public:
    explicit SceneManifest(const Path& filename);

    void Build() {}
    void Rebuild() {}
    void Clean() {}
};