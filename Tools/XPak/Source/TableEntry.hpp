// Author: Jake Rieger
// Created: 12/3/2024.
//

#pragma once

#include <Types.hpp>
#include <Panic.hpp>
#include <cstring>

struct TableEntry {
    char Name[64];
    u32 Size;
    u32 Offset;

    TableEntry() {
        memset(Name, 0, 64);
        Size   = 0;
        Offset = 0;
    }

    TableEntry(const char* name, u32 size, u32 offset) {
        const auto nameLen = strlen(name);
        if (nameLen > 63) { Panic("SceneEntry name is too long"); }
        strcpy(Name, name);
        Name[nameLen] = '\0';
        Size          = size;
        Offset        = offset;
    }
};