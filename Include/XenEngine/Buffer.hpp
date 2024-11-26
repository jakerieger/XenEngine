// Author: Jake Rieger
// Created: 11/25/2024.
//

#pragma once

#include <Panic.hpp>
#include <Types.hpp>
#include <vector>
#include <glad/glad.h>

namespace Xen {
    template<typename T>
    struct BufferDescriptor {
        u32 Type  = GL_ARRAY_BUFFER;
        u32 Usage = GL_STATIC_DRAW;
        std::vector<T> Data;

        std::size_t Size() {
            return sizeof(T) * Data.size();
        }
    };

    class Buffer {
    public:
        template<typename T>
        static u32 CreateBuffer(BufferDescriptor<T>& descriptor) {
            u32 buffer;
            glGenBuffers(1, &buffer);
            if (buffer == 0) { Panic("Failed to create buffer"); }
            glBindBuffer(descriptor.Type, buffer);
            glBufferData(descriptor.Type,
                         descriptor.Size(),
                         descriptor.Data.data(),
                         descriptor.Usage);
            glBindBuffer(descriptor.Type, 0);
            return buffer;
        }

        static void DeleteBuffer(u32 buffer) {
            glDeleteBuffers(1, &buffer);
        }
    };
}  // namespace Xen