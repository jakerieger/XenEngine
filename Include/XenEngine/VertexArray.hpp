// Author: Jake Rieger
// Created: 11/25/2024.
//

#pragma once

#include "Buffer.hpp"

#include <Panic.hpp>
#include <Types.hpp>
#include <vector>
#include <glad/glad.h>

namespace Xen {
    struct VertexAttribute {
        cstr Name;
        u32 Location;
        i32 Size;
        u32 Type;
        u32 Normalized;
        i32 Stride;
        void* Offset;
    };

    class VertexArray {
    public:
        VertexArray() {
            glGenVertexArrays(1, &mVAO);
            if (mVAO == 0) { Panic("Vertex Array creation failed"); }
        }

        ~VertexArray() {
            if (!mVBOs.empty()) glDeleteBuffers(CAST<GLsizei>(mVBOs.size()), mVBOs.data());
            if (!mEBOs.empty()) glDeleteBuffers(CAST<GLsizei>(mEBOs.size()), mEBOs.data());
            glDeleteVertexArrays(1, &mVAO);
        }

        void Bind() const {
            glBindVertexArray(mVAO);
        }

        static void Unbind() {
            glBindVertexArray(0);
        }

        //                      loc size type    normalized      stride          offset
        // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        template<typename T>
        void CreateVertexBuffer(const std::vector<T>& vertices,
                                std::vector<VertexAttribute>& attribs) {
            BufferDescriptor<T> vboDescriptor = {};
            vboDescriptor.Data                = vertices;
            const auto vbo                    = Buffer::CreateBuffer(vboDescriptor);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            for (auto i = 0; i < attribs.size(); ++i) {
                glEnableVertexAttribArray(i);
                const auto attr = attribs[i];
                glVertexAttribPointer(attr.Location,
                                      attr.Size,
                                      attr.Type,
                                      attr.Normalized,
                                      attr.Stride,
                                      attr.Offset);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mVBOs.push_back(vbo);
        }

        template<typename T>
        void CreateElementBuffer(const std::vector<T>& indices) {
            auto eboDescriptor = BufferDescriptor<f32>();
            eboDescriptor.Type = GL_ELEMENT_ARRAY_BUFFER;
            eboDescriptor.Data = indices.data();
            const auto ebo     = Buffer::CreateBuffer(eboDescriptor);
            mEBOs.push_back(ebo);
        }

    private:
        u32 mVAO = 0;
        std::vector<u32> mVBOs;
        std::vector<u32> mEBOs;
    };
}  // namespace Xen