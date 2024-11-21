// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <iostream>
#include <lzma.h>

class Compression {
public:
    static std::optional<std::vector<uint8_t>> Compress(const std::vector<uint8_t>& data) {
        if (data.empty()) { return {}; }
        lzma_stream stream = LZMA_STREAM_INIT;
        lzma_ret ret       = lzma_easy_encoder(&stream, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
        if (ret != LZMA_OK) {
            std::cerr << "LZMA error: " << ret << std::endl;
            return {};
        }

        std::vector<uint8_t> compressed;
        compressed.resize(data.size() + data.size() / 3 + 128);

        stream.next_in   = data.data();
        stream.avail_in  = data.size();
        stream.next_out  = compressed.data();
        stream.avail_out = compressed.size();

        // Do the actual compression
        ret = lzma_code(&stream, LZMA_FINISH);
        if (ret != LZMA_STREAM_END) {
            lzma_end(&stream);
            std::cerr << "LZMA error: " << ret << std::endl;
            return {};
        }

        compressed.resize(stream.total_out);
        lzma_end(&stream);
        return compressed;
    }

    static std::optional<std::vector<uint8_t>> Decompress(const std::vector<uint8_t>& data,
                                                          const size_t originalSize) {
        if (data.empty()) { return {}; }

        lzma_stream stream = LZMA_STREAM_INIT;
        lzma_ret ret       = lzma_auto_decoder(&stream, UINT64_MAX, 0);
        if (ret != LZMA_OK) {
            std::cerr << "LZMA error: " << ret << std::endl;
            return {};
        }

        std::vector<uint8_t> decompressed(originalSize);

        stream.next_in   = data.data();
        stream.avail_in  = data.size();
        stream.next_out  = decompressed.data();
        stream.avail_out = decompressed.size();

        // Do the actual decompression
        ret = lzma_code(&stream, LZMA_FINISH);
        if (ret != LZMA_STREAM_END) {
            lzma_end(&stream);
            std::cerr << "LZMA error: " << ret << std::endl;
            return {};
        }

        lzma_end(&stream);  // Clean up
        return decompressed;
    }
};