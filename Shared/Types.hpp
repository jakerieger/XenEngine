// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#pragma region Primitives
#include <cstdint>
#include <cstring>
#include <string>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

#if defined(__GNUC__) || defined(__clang__)
using u128 = __uint128_t;
using i128 = __int128_t;
#endif

using f32 = float;
using f64 = double;

using cstr = const char*;
using str  = std::string;
using wstr = std::wstring;
#pragma endregion

#define CAST static_cast
#define CCAST const_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast

#include <memory>

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename D>
using UniqueDelete = std::unique_ptr<T, D>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Weak = std::weak_ptr<T>;

static f32 ToFloat(const str& val) {
    return std::stof(val);
}

// Allows you to define std::string literals the same way as char* pointers,
// simply append "s" to the end of your double-quotes: "string"s
using std::operator""s;

static f32 ToFloat(const char* val) {
    char* end;
    return strtof(val, &end);
}

static f32 ToFloat(const wstr& val) {
    return std::stof(val);
}

static u32 ToUInt(const str& val) {
    return std::stoul(val);
}

static u32 ToUInt(const char* val) {
    char* end;
    return strtoul(val, &end, 0);
}

static u32 ToUInt(const wstr& val) {
    return std::stoul(val);
}

static i32 ToInt(const str& val) {
    return std::stoi(val);
}

static i32 ToInt(const char* val) {
    char* end;
    return CAST<i32>(strtol(val, &end, 0));
}

static i32 ToInt(const wstr& val) {
    return std::stoi(val);
}

static bool ToBool(const str& val) {
    return val == "true" || val == "True";
}

static bool ToBool(const char* val) {
    return strcmp(val, "true") == 0 || strcmp(val, "True") == 0;
}