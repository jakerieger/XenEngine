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