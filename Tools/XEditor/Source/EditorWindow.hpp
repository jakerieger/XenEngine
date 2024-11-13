// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Types/Types.h>
#include <Types/Cast.h>
#include <Types/SmartPtr.h>

#include "EditorUI.hpp"
#include "Panic.hpp"
class EditorWindow {
public:
    EditorWindow(int width, int height) {
        if (!glfwInit()) { Panic("Failed to initialize GLFW"); }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

        window = glfwCreateWindow(width, height, "XEditor — Untitled", nullptr, nullptr);
        if (!window) { Panic("Failed to create GLFW window"); }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
            Panic("Failed to initialize OpenGL context");
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(window,
                                       [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });

        glfwSwapInterval(1);

        ui = std::make_unique<EditorUI>(&window);
    }

    ~EditorWindow() {
        ui.reset();
        if (window) { glfwDestroyWindow(window); }
    }

    void Run() const {
        glClearColor(0.f, 0.f, 0.f, 1.0);
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            EditorUI::BeginUI();
            ui->Draw();
            EditorUI::EndUI();

            glfwSwapBuffers(window);
        }
    }

private:
    GLFWwindow* window;
    Unique<EditorUI> ui;
};