// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

class IO {
public:
#pragma region Read
    static std::optional<std::vector<uint8_t>> ReadBytes(const std::filesystem::path& filename) {
        if (!exists(filename)) { return {}; }
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) { return {}; }
        const std::streamsize fileSize = file.tellg();
        std::vector<uint8_t> bytes(fileSize);
        file.seekg(0, std::ios::beg);
        if (!file.read(RCAST<char*>(bytes.data()), fileSize)) { return {}; }
        file.close();

        return bytes;
    }

    static std::string ReadString(const std::filesystem::path& filename) {
        if (!exists(filename)) { return {}; }
        const std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
#pragma endregion

#pragma region Write
    static bool WriteBytes(const std::filesystem::path& filename,
                           const std::vector<uint8_t>& bytes) {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) { return false; }
        file.write(RCAST<const char*>(bytes.data()), bytes.size());
        file.close();
        return true;
    }

    static bool WriteString(const std::filesystem::path& filename, const std::string& str) {
        std::ofstream file(filename);
        if (!file.is_open()) { return false; }
        file << str;
        file.close();
        return true;
    }
#pragma endregion
};