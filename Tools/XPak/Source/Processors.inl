// Author: Jake Rieger
// Created: 11/14/2024.
//

#pragma once

class Processors {
public:
    // TODO: Write the actual implementations for these

    static Vector<u8> ProcessTexture(const Path& filename) {
        return ReadFile(filename);
    }

    static Vector<u8> ProcessAudio(const Path& filename) {
        return ReadFile(filename);
    }

    static Vector<u8> ProcessData(const Path& filename) {
        return ReadFile(filename);
    }

private:
    static Vector<u8> ReadFile(const Path& filename) {
        if (!exists(filename)) { Panic("File not found"); }
        std::ifstream file(filename, std::ios::binary | std::ios::ate);  // ate -> seeks to end
        if (!file.is_open()) { Panic("Failed to open file"); }
        const std::streamsize fileSize = file.tellg();
        if (fileSize <= 0) { Panic("File size invalid ( <= zero )"); }
        Vector<u8> buffer(fileSize);
        file.seekg(0, std::ios::beg);  // Move back to beginning of file
        if (!file.read(RCAST<char*>(buffer.data()), fileSize)) { Panic("Failed to read file"); }
        file.close();
        return buffer;
    }
};