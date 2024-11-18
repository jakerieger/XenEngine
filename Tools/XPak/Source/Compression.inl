// Author: Jake Rieger
// Created: 11/18/2024.
//

#pragma once

#define TYPES_ALL
#define INC_VECTOR
#define INC_OPTION
#include <iostream>
#include <Types/Types.h>
#include <lzma.h>

namespace Compression {
    static constexpr auto None = std::nullopt;

    static Option<Vector<u8>> Compress(const Vector<u8>& data) {
        if (data.empty()) { return None; }
        lzma_stream stream = LZMA_STREAM_INIT;
        lzma_ret ret       = lzma_easy_encoder(&stream, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
        if (ret != LZMA_OK) {
            std::cerr << "LZMA error: " << ret << std::endl;
            return None;
        }

        Vector<u8> compressed;
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
            return None;
        }

        compressed.resize(stream.total_out);
        lzma_end(&stream);
        return compressed;
    }

    static Option<Vector<u8>> Decompress(const Vector<u8>& data, const size_t originalSize) {
        if (data.empty()) { return None; }

        lzma_stream stream = LZMA_STREAM_INIT;
        lzma_ret ret       = lzma_auto_decoder(&stream, UINT64_MAX, 0);
        if (ret != LZMA_OK) {
            std::cerr << "LZMA error: " << ret << std::endl;
            return None;
        }

        Vector<u8> decompressed(originalSize);

        stream.next_in   = data.data();
        stream.avail_in  = data.size();
        stream.next_out  = decompressed.data();
        stream.avail_out = decompressed.size();

        // Do the actual decompression
        ret = lzma_code(&stream, LZMA_FINISH);
        if (ret != LZMA_STREAM_END) {
            lzma_end(&stream);
            std::cerr << "LZMA error: " << ret << std::endl;
            return None;
        }

        lzma_end(&stream);  // Clean up
        return decompressed;
    }
}  // namespace Compression