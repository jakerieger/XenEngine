// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include "EditorStyle.hpp"
#include "XenEngine.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#define EXIT_CODE_QUIT 69

namespace XEditor {
    class EditorUI {
    public:
        explicit EditorUI(GLFWwindow** window) : activeScene("Untitled") {
            editorWindow = window;

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
            io.ConfigFlags |=
              ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows

            // Load default theme
            EditorStyle::LoadAndApplyStyle("XenDark.xml");

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
            const ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* backupCtx = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backupCtx);
            }
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
        GLFWwindow** editorWindow;

        Xen::Scene activeScene;
        str activeSceneFile;

        char newSceneName[64]      = {'\0'};
        bool newSceneDialog        = false;
        char currentSceneName[64]  = {'\0'};
        char newGameObjectName[64] = {'\0'};
        str selectedGameObject;
        Vector<str> components     = {"Behavior",
                                      "Sprite Renderer",
                                      "Rigidbody",
                                      "Box Collider",
                                      "Circle Collider",
                                      "Polygon Collider",
                                      "Camera",
                                      "Audio Player"};
        int selectedComponentIndex = -1;

    private:
        void MainMenu() {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("New Scene")) { newSceneDialog = true; }

                    if (ImGui::MenuItem("Open Scene")) {
                        // File open
                        const auto title       = "Open Scene File";
                        const char* patterns[] = {"*.xscene", "*.xml"};
                        const char* selectedFile =
                          tinyfd_openFileDialog(title, "", 2, patterns, nullptr, 0);

                        if (selectedFile) {
                            activeScene     = Xen::Scene::Load(selectedFile);
                            activeSceneFile = selectedFile;
                            UpdateWindowTitle();
                        }
                    }

                    if (ImGui::MenuItem("Save Scene")) {
                        if (!activeSceneFile.empty()) { activeScene.Save(activeSceneFile.c_str()); }
                    }

                    if (ImGui::MenuItem("Save Scene As...")) { SaveSceneToFile(); }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit")) { std::exit(EXIT_CODE_QUIT); }

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }

                if (ImGui::BeginMenu("View")) { ImGui::EndMenu(); }

                if (ImGui::BeginMenu("Help")) { ImGui::EndMenu(); }

                ImGui::EndMainMenuBar();
            }
            ImGui::PopStyleVar();

            if (newSceneDialog) {
                ImGui::OpenPopup("New Scene");
                if (ImGui::BeginPopupModal("New Scene",
                                           nullptr,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Enter a name for the new scene:");
                    ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                    ImGui::InputText("##SceneName", newSceneName, IM_ARRAYSIZE(newSceneName));

                    if (ImGui::Button("OK", ImVec2(120, 0))) {
                        if (newSceneName[0] != '\0') {
                            activeScene     = Xen::Scene(newSceneName);
                            activeSceneFile = "";
                            UpdateWindowTitle();
                            SaveSceneToFile();
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }

                if (!ImGui::IsPopupOpen("New Scene")) { newSceneDialog = false; }
            }
        }

        void Toolbar() {}

        void SceneTree() {
            ImGui::Begin("Hierarchy");

            ImGui::Text(activeScene.Name.c_str());
            ImGui::SameLine(ImGui::GetColumnWidth() - 24);
            if (ImGui::Button("+", ImVec2(32, 32))) { ImGui::OpenPopup("Add new GameObject"); }

            // GameObject tree
            ImGui::BeginChild("GameObjects", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
            for (const auto& name : activeScene.GameObjects | std::views::keys) {
                if (ImGui::Selectable(name.c_str(), selectedGameObject == name)) {
                    selectedGameObject = name;
                }
            }
            ImGui::EndChild();

            if (ImGui::BeginPopupModal("Add new GameObject",
                                       nullptr,
                                       ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Enter a name for the new GameObject:");
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                ImGui::InputText("##GoName", newGameObjectName, IM_ARRAYSIZE(newSceneName));

                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    if (!activeScene.Name.empty()) {
                        activeScene.GameObjects.insert_or_assign(newGameObjectName,
                                                                 Xen::GameObject());
                        activeScene.GameObjects[newGameObjectName].Components.insert_or_assign(
                          "Transform",
                          new Xen::Transform());
                    }

                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            ImGui::End();
        }

        void GameObjectInspector() {
            ImGui::Begin("Inspector");

            if (!selectedGameObject.empty()) {
                ImGui::Text("Name: %s", selectedGameObject.c_str());
                auto gameObject = activeScene.GameObjects[selectedGameObject];
                for (auto& [name, component] : gameObject.Components) {
                    if (name == "Transform") {
                        ImGui::BeginChild("Transform",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        ImGui::Text("Transform");
                        ImGui::Text("X: %f", ((Xen::Transform*)component)->X);
                        ImGui::Text("Y: %f", ((Xen::Transform*)component)->Y);
                        ImGui::EndChild();
                    }

                    if (name == "Behavior") {
                        ImGui::BeginChild("Behavior",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        ImGui::Text("Behavior");
                        ImGui::Text("Script: %s", ((Xen::Behavior*)component)->Script.c_str());
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            activeScene.GameObjects[selectedGameObject].RemoveComponent("Behavior");
                        }
                        ImGui::EndChild();
                    }
                }

                if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 32))) {
                    selectedComponentIndex = 0;
                    ImGui::OpenPopup("Add new Component");
                }

                if (ImGui::BeginPopupModal("Add new Component",
                                           nullptr,
                                           ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Select a component below:");
                    ImGui::BeginChild("Transform",
                                      ImVec2(0, 0),
                                      ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                    // List of components
                    int idx = 0;
                    for (const auto& component : components) {
                        if (ImGui::Selectable(component.c_str(), idx == selectedComponentIndex)) {
                            // Update selected index
                            selectedComponentIndex = idx;
                        }
                        idx++;
                    }
                    ImGui::EndChild();

                    if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                    ImGui::EndPopup();
                }
            }

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

        void UpdateWindowTitle() const {
            std::stringstream ss;
            ss << "XEditor — ";
            ss << activeScene.Name;
            glfwSetWindowTitle(*editorWindow, ss.str().c_str());
        }

        void SaveSceneToFile() {
            const auto title            = "Save Scene";
            const char* defaultFileName = activeScene.Name.c_str();
            const char* patterns[]      = {"*.xscene", "*.xml"};
            const char* saveFile =
              tinyfd_saveFileDialog(title, defaultFileName, 2, patterns, nullptr);

            if (saveFile) {
                activeScene.Save(saveFile);
                activeSceneFile = saveFile;
            }
        }
    };
}  // namespace XEditor
