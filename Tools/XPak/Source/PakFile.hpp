// Author: Jake Rieger
// Created: 11/18/2024.
//

#pragma once
#include <cstring>

// Pak File Structure
//
// +---------------+---------+--------+
// | Name          | Size    | Offset |
// +---------------+---------+--------+
// | Magic         | 4       | 0      |
// +---------------+---------+--------+
// | Padding       | 3       | 4      |
// +---------------+---------+--------+
// | Compress      | 1       | 7      |
// +---------------+---------+--------+
// | Original Size | 8       | 8      |
// +---------------+---------+--------+
// | Data          | Dynamic | 16     |
// +---------------+---------+--------+
class PakFile {
public:
    static bool
    Write(std::vector<u8>& data, const std::filesystem::path& outPath, const bool compress, const size_t originalSize) {
        constexpr char magic[4]   = {'X', 'P', 'A', 'K'};
        constexpr char padding[3] = {0x0, 0x0, 0x0};

        std::vector<u8> pak(data.size() + 16);
        memcpy(pak.data(), magic, 4);
        memcpy(pak.data() + 4, padding, 3);
        memcpy(pak.data() + 7, (u8*)&compress, 1);
        memcpy(pak.data() + 8, &originalSize, 8);
        std::ranges::copy(data, pak.data() + 16);

        std::ofstream outFile(outPath, std::ios::binary);
        outFile.write(reinterpret_cast<char*>(pak.data()), (std::streamsize)pak.size());
        outFile.close();

        return true;
    }
};