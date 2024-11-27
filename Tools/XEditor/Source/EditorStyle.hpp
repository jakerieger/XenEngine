// Author: Jake Rieger
// Created: 11/11/2024.
//

#pragma once

#include "Panic.hpp"

#include <Types.hpp>
#include <filesystem>
#include <imgui/imgui.h>
#include <pugixml.hpp>

namespace EditorStyle {
    ImVec4 HexToVec4(const char* hex, f32 alpha = 1.f);
    void LoadAndApplyStyle(const char* filename, bool loadFont = true);
}  // namespace EditorStyle