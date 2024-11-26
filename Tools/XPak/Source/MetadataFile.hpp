// Author: Jake Rieger
// Created: 11/26/2024.
//

#pragma once

#include <Types.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>

class MetadataFile {
public:
    static bool Write(const std::filesystem::path& outPath,
                      const std::unordered_map<str, str>& metadata) {
        pugi::xml_document doc;
        auto rootNode = doc.append_child("Metadata");
        for (const auto [key, val] : metadata) {  // NOLINT(*-for-range-copy)
            auto node = rootNode.append_child(key.c_str());
            node.text().set(val.c_str());
        }

        std::stringstream xmlStream;
        pugi::xml_writer_stream writer(xmlStream);
        doc.save(writer, "\t", pugi::format_default, pugi::encoding_utf8);
        std::ofstream outFile(outPath);
        outFile << xmlStream.str();
        outFile.close();

        return true;
    }
};