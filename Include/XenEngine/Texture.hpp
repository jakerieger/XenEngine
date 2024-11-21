// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#define TYPES_ALL
#define INC_VECTOR
#include <Types/Types.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <Panic.hpp>

namespace Xen {
    class Texture {
    public:
        static u32 GetGLSlot(u32 slot) {
            if (slot > kMaxSlot) { Panic("Slot is out of range! Must be (> 0) and (< 32)"); }
            return GL_TEXTURE0 + slot;
        }

        static u32 LoadFromFile(const char* filename, int* width = nullptr, int* height = nullptr) {
            u32 id;
            glGenTextures(1, &id);

            int w, h, channels;
            stbi_set_flip_vertically_on_load(true);  // For OpenGL
            auto data = stbi_load(filename, &w, &h, &channels, 0);
            if (!data) { Panic("Failed to load image!"); }

            if (w) *width = w;
            if (h) *height = h;

            GLenum format = GL_RGBA;
            if (channels == 1) format = GL_RED;
            else if (channels == 3) format = GL_RGB;
            else if (channels == 4) format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         CAST<int>(format),
                         w,
                         h,
                         0,
                         format,
                         GL_UNSIGNED_BYTE,
                         data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_S,
                            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,
                            GL_TEXTURE_WRAP_T,
                            format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            stbi_image_free(data);

            return id;
        }

        /// @brief Loads a texture from data stored in memory. Assumes RGBA color format (4 bytes
        /// per pixel). See XPak source for specific details on how textures are processed for
        /// in-memory loading.
        static u32
        LoadFromMemory(const Vector<u8>& data, int* width = nullptr, int* height = nullptr) {
            return 0;
        }

        static void Delete(u32 id) {
            glDeleteTextures(1, &id);
        }

        static void Bind(u32 id, u32 slot) {
            glBindTexture(GL_TEXTURE_2D, id);
            glActiveTexture(GetGLSlot(slot));
        }

        static void Unbind() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        static constexpr auto kMaxSlot = 31;
    };
}  // namespace Xen