// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include "XenEngine.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace XEditor {
    class EditorUI {
    public:
        explicit EditorUI(GLFWwindow** window) : activeScene("Untitled") {
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
            ImGui::DockSpaceOverViewport(0,
                                         ImGui::GetMainViewport(),
                                         ImGuiDockNodeFlags_PassthruCentralNode);

            MainMenu();
            Toolbar();
            SceneTree();
            GameObjectInspector();
            SceneViewport();
            DebugConsole();
        }

    private:
        Xen::Scene activeScene;

    private:
        void MainMenu() {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) { ImGui::EndMenu(); }

                if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }

                if (ImGui::BeginMenu("View")) { ImGui::EndMenu(); }

                if (ImGui::BeginMenu("Help")) { ImGui::EndMenu(); }

                ImGui::EndMainMenuBar();
            }

            ImGui::PopStyleVar();
        }

        void Toolbar() {}

        void SceneTree() {
            ImGui::Begin("Hierarchy");
            ImGui::Text("Name: %s", activeScene.Name.c_str());
            ImGui::End();
        }

        void GameObjectInspector() {
            ImGui::Begin("Inspector");
            ImGui::End();
        }

        void SceneViewport() {
            ImGui::Begin("Scene");
            ImGui::End();
        }

        void DebugConsole() {
            ImGui::Begin("Messages");
            ImGui::End();
        }
    };
}  // namespace XEditor
