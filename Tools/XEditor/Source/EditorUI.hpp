// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace XEditor {
    class EditorUI {
    public:
        explicit EditorUI(GLFWwindow** window) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
            // io.ConfigFlags |=
            //   ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows

            ImGui::StyleColorsDark();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can
            // look identical to regular ones.
            // ImGuiStyle& style = ImGui::GetStyle();
            // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            //     style.WindowRounding              = 0.0f;
            //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            // }

            ImGui_ImplGlfw_InitForOpenGL(*window, true);
            ImGui_ImplOpenGL3_Init("#version 130");
        }

        ~EditorUI() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        static void BeginUI() {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        static void EndUI() {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to
            // make it easier to paste this code elsewhere.
            // const ImGuiIO& io = ImGui::GetIO();
            // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            //     GLFWwindow* backupCtx = glfwGetCurrentContext();
            //     ImGui::UpdatePlatformWindows();
            //     ImGui::RenderPlatformWindowsDefault();
            //     glfwMakeContextCurrent(backupCtx);
            // }
        }

        void Draw() {
            MainMenu();
            Toolbar();
            SceneTree();
            GameObjectInspector();
            SceneViewport();
        }

    private:
        void MainMenu() {}

        void Toolbar() {}

        void SceneTree() {
            ImGui::Begin("Scene");
            ImGui::End();
        }

        void GameObjectInspector() {
            ImGui::Begin("Inspector");
            ImGui::End();
        }

        void SceneViewport() {
            ImGui::Begin("Scene Preview");
            ImGui::End();
        }
    };
}  // namespace XEditor
