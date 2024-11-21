// Author: Jake Rieger
// Created: 11/21/2024.
//

#pragma once

#define TYPES_ALL
#include <Types/Types.h>

#include <glm/glm.hpp>

namespace Xen {
    /// @brief Encapsulates an OpenGL vertex/fragment shader program.
    ///
    /// @warning Cannot be used for compute or geometry shaders.
    class Shader {
    public:
        Shader(const char* vertexSource, const char* fragmentSource);
        Shader(const Path& vertexPath, const Path& fragmentPath);
        ~Shader();

        /// @brief Binds this shader to the OpenGL context
        void Bind() const;

        /// @brief Unbinds any currently bound shader.
        static void Unbind();

        void SetBool(const char* name, bool value) const;
        void SetInt(const char* name, int value) const;
        void SetFloat(const char* name, f32 value) const;
        void SetVec2(const char* name, const glm::vec2& value) const;
        void SetVec2(const char* name, f32 x, f32 y) const;
        void SetVec3(const char* name, const glm::vec3& value) const;
        void SetVec3(const char* name, f32 x, f32 y, f32 z) const;
        void SetVec4(const char* name, const glm::vec4& value) const;
        void SetVec4(const char* name, f32 x, f32 y, f32 z, f32 w) const;
        void SetMat2(const char* name, const glm::mat2& mat) const;
        void SetMat3(const char* name, const glm::mat3& mat) const;
        void SetMat4(const char* name, const glm::mat4& mat) const;

        template<typename... Args>
        static Unique<Shader> Create(Args&&... args) {
            return std::make_unique<Shader>(std::forward<Args>(args)...);
        }

    private:
        void CompileShaders(const char* vertexSource, const char* fragmentSource);
        static void CheckErrors(u32 handle, bool isProgram = false);
        u32 mProgramID = 0;
    };
}  // namespace Xen