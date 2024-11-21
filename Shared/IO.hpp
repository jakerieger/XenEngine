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
};