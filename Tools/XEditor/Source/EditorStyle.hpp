// Author: Jake Rieger
// Created: 11/11/2024.
//

#pragma once

#include "Panic.hpp"

#include <Types/Types.h>
#include <Types/Cast.h>
#include <Types/IO.h>
#include <imgui/imgui.h>
#include <concepts>
#include <cmath>
#include <pugixml.hpp>

namespace EditorStyle {
    inline void HexToRGBA(const u32 hex, f32& r, f32& g, f32& b, f32& a) {
        const u8 alphaByte = (hex >> 24) & 0xFF;
        const u8 redByte   = (hex >> 16) & 0xFF;
        const u8 greenByte = (hex >> 8) & 0xFF;
        const u8 blueByte  = hex & 0xFF;

        a = CAST<f32>(CAST<u32>(alphaByte) / 255.0);
        r = CAST<f32>(CAST<u32>(redByte) / 255.0);
        g = CAST<f32>(CAST<u32>(greenByte) / 255.0);
        b = CAST<f32>(CAST<u32>(blueByte) / 255.0);
    }

    inline void HexToRGBA(const u32 hex, u32& r, u32& g, u32& b, u32& a) {
        const u8 alphaByte = (hex >> 24) & 0xFF;
        const u8 redByte   = (hex >> 16) & 0xFF;
        const u8 greenByte = (hex >> 8) & 0xFF;
        const u8 blueByte  = hex & 0xFF;

        a = CAST<u32>(alphaByte);
        r = CAST<u32>(redByte);
        g = CAST<u32>(greenByte);
        b = CAST<u32>(blueByte);
    }

    inline u32 RGBAToHex(const f32 r, const f32 g, const f32 b, const f32 a) {
        const auto redByte   = CAST<u8>(r * 255.0f);
        const auto greenByte = CAST<u8>(g * 255.0f);
        const auto blueByte  = CAST<u8>(b * 255.0f);
        const auto alphaByte = CAST<u8>(a * 255.0f);

        return (alphaByte << 24) | (redByte << 16) | (greenByte << 8) | blueByte;
    }

    inline ImVec4 HexToVec4(
      const char* hex,
      const f32 alpha = 1.f) {  // Ensure the string starts with '#' and is the correct length
        if (std::strlen(hex) != 6) {
            throw std::invalid_argument("Hex color should be in the format #RRGGBB");
        }

        ImVec4 color;
        int r, g, b;
        sscanf(hex, "%02x%02x%02x", &r, &g, &b);

        color.x = (float)r / 255.0f;
        color.y = (float)g / 255.0f;
        color.z = (float)b / 255.0f;
        color.w = alpha;

        return color;
    }

    namespace Math {
        template<typename T>
        concept FiniteFloat = std::floating_point<T> && requires(T value) {
            { std::isfinite(value) } -> std::convertible_to<bool>;
        };

        template<FiniteFloat T>
        T Lerp(T a, T b, double t) {
            if (a == b) { return a; }

            auto result = a * (1.0 - t) + b * t;
            return CAST<T>(result);
        }
    }  // namespace Math

    static void LoadAndApplyStyle(const char* filename) {
        // Parse XML file
        pugi::xml_document doc;
        const Path themeFile   = Path("Themes") / filename;
        const auto parseResult = doc.load_file(themeFile.c_str());
        if (!parseResult) { Panic("Failed to load theme file"); }

        const auto themeRoot = doc.child("Theme");
        const auto themeName = themeRoot.attribute("name").value();
        const auto fontFile  = themeRoot.child_value("Font");

        // Load font
        const ImGuiIO& io   = ImGui::GetIO();
        const auto fontPath = Path("Themes/Fonts") / fontFile;
        io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16);

        const auto borderRadiusValue = themeRoot.child_value("BorderRadius");
        const auto borderWidthValue  = themeRoot.child_value("BorderWidth");
        char* end;
        const f32 borderRadius = CAST<f32>(strtod(borderRadiusValue, &end));
        const f32 borderWidth  = CAST<f32>(strtod(borderWidthValue, &end));

        ImGuiStyle& style   = ImGui::GetStyle();
        ImVec4* styleColors = style.Colors;

        style.WindowRounding   = borderRadius;
        style.FrameRounding    = borderRadius;
        style.WindowBorderSize = borderWidth;
        style.FrameBorderSize  = 0.f;
        style.TabRounding      = borderRadius;

        // Load colors
        const auto windowBackground    = HexToVec4(themeRoot.child_value("WindowBackground"));
        const auto childBackground     = HexToVec4(themeRoot.child_value("ChildBackground"));
        const auto frameBackground     = HexToVec4(themeRoot.child_value("FrameBackground"));
        const auto secondaryBackground = HexToVec4(themeRoot.child_value("SecondaryBackground"));
        const auto headerBackground    = HexToVec4(themeRoot.child_value("HeaderBackground"));
        const auto textHighlight       = HexToVec4(themeRoot.child_value("TextHighlight"));
        const auto textPrimary         = HexToVec4(themeRoot.child_value("TextPrimary"));
        const auto textSecondary       = HexToVec4(themeRoot.child_value("TextSecondary"));
        const auto textDisabled        = HexToVec4(themeRoot.child_value("TextDisabled"));
        const auto border              = HexToVec4(themeRoot.child_value("Border"));
        const auto error               = HexToVec4(themeRoot.child_value("Error"));
        const auto warning             = HexToVec4(themeRoot.child_value("Warning"));
        const auto success             = HexToVec4(themeRoot.child_value("Success"));
        const auto link                = HexToVec4(themeRoot.child_value("Link"));
        const auto scrollbar           = HexToVec4(themeRoot.child_value("Scrollbar"));
        const auto primary             = HexToVec4(themeRoot.child_value("Primary"));
        const auto secondary           = HexToVec4(themeRoot.child_value("Secondary"));

        styleColors[ImGuiCol_Text]                 = textPrimary;
        styleColors[ImGuiCol_TextDisabled]         = textDisabled;
        styleColors[ImGuiCol_WindowBg]             = windowBackground;
        styleColors[ImGuiCol_ChildBg]              = childBackground;
        styleColors[ImGuiCol_PopupBg]              = windowBackground;
        styleColors[ImGuiCol_Border]               = border;
        styleColors[ImGuiCol_BorderShadow]         = ImVec4(0.f, 0.f, 0.f, 0.f);
        styleColors[ImGuiCol_FrameBg]              = frameBackground;
        styleColors[ImGuiCol_FrameBgHovered]       = secondaryBackground;
        styleColors[ImGuiCol_FrameBgActive]        = secondaryBackground;
        styleColors[ImGuiCol_TitleBg]              = frameBackground;
        styleColors[ImGuiCol_TitleBgActive]        = frameBackground;
        styleColors[ImGuiCol_TitleBgCollapsed]     = frameBackground;
        styleColors[ImGuiCol_MenuBarBg]            = windowBackground;
        styleColors[ImGuiCol_ScrollbarBg]          = windowBackground;
        styleColors[ImGuiCol_ScrollbarGrab]        = scrollbar;
        styleColors[ImGuiCol_ScrollbarGrabHovered] = scrollbar;
        styleColors[ImGuiCol_ScrollbarGrabActive]  = scrollbar;
        styleColors[ImGuiCol_CheckMark]            = textPrimary;
        styleColors[ImGuiCol_SliderGrab]           = scrollbar;
        styleColors[ImGuiCol_SliderGrabActive]     = scrollbar;
        styleColors[ImGuiCol_Button]               = secondary;
        styleColors[ImGuiCol_ButtonHovered] = ImVec4(secondary.x, secondary.y, secondary.z, 0.75f);
        styleColors[ImGuiCol_ButtonActive]  = ImVec4(secondary.x, secondary.y, secondary.z, 0.60f);
        styleColors[ImGuiCol_Header]        = secondaryBackground;
        styleColors[ImGuiCol_HeaderHovered] = headerBackground;
        styleColors[ImGuiCol_HeaderActive]  = headerBackground;
        styleColors[ImGuiCol_Separator]     = border;
        styleColors[ImGuiCol_SeparatorHovered]     = link;
        styleColors[ImGuiCol_SeparatorActive]      = link;
        styleColors[ImGuiCol_ResizeGrip]           = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
        styleColors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        styleColors[ImGuiCol_ResizeGripActive]     = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        styleColors[ImGuiCol_Tab]                  = headerBackground;
        styleColors[ImGuiCol_TabHovered]           = headerBackground;
        styleColors[ImGuiCol_TabActive]            = headerBackground;
        styleColors[ImGuiCol_TabUnfocused]         = styleColors[ImGuiCol_Tab];
        styleColors[ImGuiCol_TabUnfocusedActive]   = styleColors[ImGuiCol_TabActive];
        styleColors[ImGuiCol_TableBorderLight]     = ImVec4(0.f, 0.f, 0.f, 0.f);
        styleColors[ImGuiCol_TableBorderStrong]    = ImVec4(0.f, 0.f, 0.f, 0.f);
        styleColors[ImGuiCol_PlotLines]            = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        styleColors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        styleColors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        styleColors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        styleColors[ImGuiCol_TableHeaderBg]        = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        styleColors[ImGuiCol_TableBorderStrong] =
          ImVec4(0.31f, 0.31f, 0.35f, 1.00f);  // Prefer using Alpha=1.0 here
        styleColors[ImGuiCol_TableBorderLight] =
          ImVec4(0.23f, 0.23f, 0.25f, 1.00f);  // Prefer using Alpha=1.0 here
        styleColors[ImGuiCol_TableRowBg]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        styleColors[ImGuiCol_TableRowBgAlt]  = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        styleColors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        styleColors[ImGuiCol_DragDropTarget] = link;
        styleColors[ImGuiCol_NavHighlight] =
          ImVec4(30.f / 255.f, 30.f / 255.f, 30.f / 255.f, 1.00f);
        styleColors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        styleColors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        styleColors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);

        std::cout << "Loaded theme: " << themeName << std::endl;
    }
}  // namespace EditorStyle