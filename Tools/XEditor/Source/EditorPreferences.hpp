// Author: Jake Rieger
// Created: 11/17/2024.
//

#pragma once

#include <Types.hpp>
#include <pugixml.hpp>
#include <Panic.hpp>

class EditorPreferences {
public:
    str Theme;
    str XPakPath;
    EditorPreferences() = default;

    void LoadFromFile(const str& path);
};
