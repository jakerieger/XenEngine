// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include "EditorUI.hpp"
#include "Panic.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Types/Types.h>
#include <Types/Cast.h>
#include <Types/SmartPtr.h>

namespace XEditor {
    class EditorWindow {
    public:
        EditorWindow(int width, int height) {
            if (!glfwInit()) { Panic("Failed to initialize GLFW"); }
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 4);
#ifndef NDEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

            window_ = glfwCreateWindow(width, height, "XEditor", nullptr, nullptr);
            if (!window_) { Panic("Failed to create GLFW window"); }

            glfwMakeContextCurrent(window_);

            if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
                Panic("Failed to initialize OpenGL context");
            }

            glViewport(0, 0, width, height);
            glfwSetFramebufferSizeCallback(window_, [](GLFWwindow*, int w, int h) {
                glViewport(0, 0, w, h);
            });

            glfwSwapInterval(1);

            ui_ = std::make_unique<EditorUI>();
        }

        ~EditorWindow() {
            ui_.reset();
            if (window_) { glfwDestroyWindow(window_); }
        }

        void Run() const {
            while (!glfwWindowShouldClose(window_)) {
                glfwPollEvents();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glfwSwapBuffers(window_);
            }
        }

    private:
        GLFWwindow* window_;
        Unique<EditorUI> ui_;
    };
}  // namespace XEditor
