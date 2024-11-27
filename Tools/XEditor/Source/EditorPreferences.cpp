// Author: Jake Rieger
// Created: 11/17/2024.
//

#include "EditorPreferences.hpp"

void EditorPreferences::LoadFromFile(const str& path) {
    pugi::xml_document doc;
    const pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result) { Panic("Couldn't load xml file"); }
    const auto& rootNode = doc.child("EditorPreferences");
    Theme                = rootNode.child_value("Theme");
    XPakPath             = rootNode.child_value("XPakPath");
}