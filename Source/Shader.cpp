// Author: Jake Rieger
// Created: 11/21/2024.
//

#include "Shader.hpp"

#include <Panic.hpp>
#include <glad/glad.h>
#include <IO.hpp>

namespace Xen {
    Shader::Shader(const char* vertexSource, const char* fragmentSource) {
        CompileShaders(vertexSource, fragmentSource);
    }

    Shader::Shader(const Path& vertexPath, const Path& fragmentPath) {
        if (!std::filesystem::exists(vertexPath) || !std::filesystem::exists(fragmentPath)) {
            Panic("Path does not exist: {}", vertexPath.string());
        }

        const auto vertexSource = IO::ReadString(vertexPath);
        if (vertexSource.empty()) Panic("Could not read vertex shader");
        const auto fragmentSource = IO::ReadString(fragmentPath);
        if (fragmentSource.empty()) Panic("Could not read fragment shader");

        CompileShaders(vertexSource.c_str(), fragmentSource.c_str());
    }

    Shader::~Shader() {
        glDeleteProgram(mProgramID);
    }

    void Shader::Bind() const {
        glUseProgram(mProgramID);
    }

    void Shader::Unbind() {
        glUseProgram(0);
    }

    void Shader::CompileShaders(const char* vertexSource, const char* fragmentSource) {
        const u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
        CheckErrors(vertexShader);

        const u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);
        CheckErrors(fragmentShader);

        mProgramID = glCreateProgram();
        glAttachShader(mProgramID, vertexShader);
        glAttachShader(mProgramID, fragmentShader);
        glLinkProgram(mProgramID);
        CheckErrors(mProgramID, true);
        glValidateProgram(mProgramID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Shader::CheckErrors(u32 handle, bool isProgram) {
        int success;
        char infoLog[1024];

        if (isProgram) {
            glGetProgramiv(handle, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(handle, 1024, nullptr, infoLog);
                Panic("glGetProgramInfoLog: %s", infoLog);
            }
        } else {
            glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

            if (!success) {
                glGetShaderInfoLog(handle, 1024, nullptr, infoLog);
                Panic("glGetShaderInfoLog: %s", infoLog);
            }
        }
    }

    void Shader::SetBool(const char* name, bool value) const {
        glUniform1i(glGetUniformLocation(mProgramID, name), static_cast<int>(value));
    }

    void Shader::SetInt(const char* name, int value) const {
        glUniform1i(glGetUniformLocation(mProgramID, name), value);
    }

    void Shader::SetFloat(const char* name, f32 value) const {
        glUniform1f(glGetUniformLocation(mProgramID, name), value);
    }

    void Shader::SetVec2(const char* name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(mProgramID, name), 1, &value[0]);
    }

    void Shader::SetVec2(const char* name, f32 x, f32 y) const {
        glUniform2f(glGetUniformLocation(mProgramID, name), x, y);
    }

    void Shader::SetVec3(const char* name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(mProgramID, name), 1, &value[0]);
    }

    void Shader::SetVec3(const char* name, f32 x, f32 y, f32 z) const {
        glUniform3f(glGetUniformLocation(mProgramID, name), x, y, z);
    }

    void Shader::SetVec4(const char* name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(mProgramID, name), 1, &value[0]);
    }

    void Shader::SetVec4(const char* name, f32 x, f32 y, f32 z, f32 w) const {
        glUniform4f(glGetUniformLocation(mProgramID, name), x, y, z, w);
    }

    void Shader::SetMat2(const char* name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(mProgramID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::SetMat3(const char* name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(mProgramID, name), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::SetMat4(const char* name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(mProgramID, name), 1, GL_FALSE, &mat[0][0]);
    }
}  // namespace Xen