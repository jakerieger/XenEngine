// Author: Jake Rieger
// Created: 10/22/2024.
//

#pragma once
#pragma warning(disable : 4201)
#pragma warning(disable : 4996)

// Use the C++ standard templated min/max
#define NOMINMAX

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Direct2D
#include <dwrite.h>
#include <d2d1.h>

// ComPtr
#include <wrl/client.h>

// DXGI
#include <d3d11_1.h>
#include <dxgi1_6.h>

// Direct3D 11
#include <d3d11.h>

// DirectX helper files
#include <DirectXMath.h>
#include <DirectXColors.h>

#ifdef _DEBUG
    #include <dxgidebug.h>
#endif

using Microsoft::WRL::ComPtr;

#include <iostream>
#include <cstdarg>

[[noreturn]] static void panic_impl(const char* file, int line, const char* func, const char* fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);
    char message[1024];
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    std::cerr << file << "(" << line << ") :: PANIC\n"
              << "  In: " << func << "\n"
              << "  Message: " << message << std::endl;
    std::abort();
}

#define Panic(fmt, ...) panic_impl(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)