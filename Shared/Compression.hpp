// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#include "Types.hpp"
#include <optional>
#include <vector>
#include <cstdint>
#include <iostream>
#include <lzma.h>
#include <zlib.h>

class LZMA {
public:
    static std::optional<std::vector<uint8_t>> Compress(const std::vector<uint8_t>& data) {
        if (data.empty()) { return {}; }
        lzma_stream stream = LZMA_STREAM_INIT;
        lzma_ret ret       = lzma_easy_encoder(&stream, LZMA_PRESET_EXTREME, LZMA_CHECK_CRC64);
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

class GZip {
public:
    static std::optional<std::vector<uint8_t>> Compress(const std::vector<uint8_t>& input) {
        if (input.empty()) { return {}; }

        uLongf compressedSize = compressBound(input.size());
        std::vector<uint8_t> compressedData(compressedSize);

        const auto result = compress(compressedData.data(),
                                     &compressedSize,
                                     RCAST<const Bytef*>(input.data()),
                                     input.size());

        if (result != Z_OK) {
            std::cerr << "Compression error: " << result << std::endl;
            return {};
        }

        compressedData.resize(compressedSize);
        return compressedData;
    }

    static std::optional<std::vector<uint8_t>> Decompress(const std::vector<uint8_t>& data,
                                                          const size_t originalSize) {
        if (data.empty() || originalSize <= 0) { return {}; }

        // Initial guess for decompressed size (can be adjusted later if needed)
        uLongf decompressedSize = data.size() * 4;  // Start with a larger buffer
        std::vector<uint8_t> decompressedData(decompressedSize);

        // Attempt to decompress the data
        int result =
          uncompress(reinterpret_cast<Bytef*>(decompressedData.data()),  // Destination buffer
                     &decompressedSize,                                  // Decompressed size
                     data.data(),                                        // Input data
                     data.size()                                         // Input size
          );

        // If the buffer was too small, resize and retry
        if (result == Z_BUF_ERROR) {
            decompressedSize *= 2;  // Double the buffer size and retry
            decompressedData.resize(decompressedSize);

            result = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()),
                                &decompressedSize,
                                data.data(),
                                data.size());
        }

        // Check if decompression was successful
        if (result != Z_OK) {
            throw std::runtime_error("Decompression failed with error code " +
                                     std::to_string(result));
        }

        return decompressedData;
    }
};