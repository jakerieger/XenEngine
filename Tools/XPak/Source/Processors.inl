// Author: Jake Rieger
// Created: 11/14/2024.
//

#pragma once

#include <stb_image.h>
#include <AudioFile.h>
#include <vector>
#include <filesystem>

class Processors {
public:
    // TODO: Write the actual implementations for these

    static std::vector<u8> ProcessTexture(const std::filesystem::path& filename) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);  // needed for OpenGL
        const stbi_uc* data =
          stbi_load(filename.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data) { Panic("Failed to load image"); }
        std::vector<u8> result(width * height * channels);
        memcpy(result.data(), data, width * height * channels);

        return result;
    }

    static std::vector<u8> ProcessAudio(const std::filesystem::path& filename) {
        AudioFile<f32> audio;
        if (!audio.load(filename.string())) {
            std::cerr << "Failed to load audio file: " << filename.string() << std::endl;
            return {};
        }
        if (audio.getNumChannels() != 2) {
            std::cerr << "Audio file does not have two channels (stereo)" << std::endl;
            return {};
        }

        size_t numSamples = audio.getNumSamplesPerChannel();
        std::vector<f32> interleavedData(numSamples * 2);
        for (auto i = 0; i < numSamples; ++i) {
            interleavedData[i * 2]     = audio.samples[0][i];  // Left channel
            interleavedData[i * 2 + 1] = audio.samples[1][i];  // Right channel
        }

        std::vector<u8> result(interleavedData.size() * 4);  // Size of float in bytes
        memcpy(result.data(), interleavedData.data(), interleavedData.size() * 4);

        return result;
    }

    static std::vector<u8> ProcessData(const std::filesystem::path& filename) {
        return ReadFile(filename);
    }

private:
    static std::vector<u8> ReadFile(const std::filesystem::path& filename) {
        if (!exists(filename)) { Panic("File not found"); }
        std::ifstream file(filename, std::ios::binary | std::ios::ate);  // ate -> seeks to end
        if (!file.is_open()) { Panic("Failed to open file"); }
        const std::streamsize fileSize = file.tellg();
        if (fileSize <= 0) { Panic("File size invalid ( <= zero )"); }
        std::vector<u8> buffer(fileSize);
        file.seekg(0, std::ios::beg);  // Move back to beginning of file
        if (!file.read(RCAST<char*>(buffer.data()), fileSize)) { Panic("Failed to read file"); }
        file.close();
        return buffer;
    }
};