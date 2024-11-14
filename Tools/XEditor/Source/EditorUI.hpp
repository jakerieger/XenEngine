// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include "EditorLog.h"
#include "EditorStyle.hpp"
#include "XenEngine.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#define EXIT_CODE_QUIT 69

static const auto DefaultTheme = "XenDark.xml";

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
        EditorStyle::LoadAndApplyStyle(DefaultTheme);

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
        // Get scene frame
        if (!activeScene.Name.empty()) {
            for (const auto& go : activeScene.GameObjects | std::views::values) {
                if (auto component = go.Components.find("Sprite Renderer");
                    component != go.Components.end()) {
                    const auto& [_, renderer] = *component;
                    renderer->As<Xen::SpriteRenderer>()->Draw();
                }
            }
        }

        ImGui::DockSpaceOverViewport(0,
                                     ImGui::GetMainViewport(),
                                     ImGuiDockNodeFlags_PassthruCentralNode);

        MainMenu();
        Toolbar();
        Hierarchy();
        Inspector();
        Scene();
        ContentBrowser();
        Messages();
    }

    void DeferredCallbacks() {
        if (deferredCallbacks.empty()) return;
        for (const auto& cb : deferredCallbacks) {
            cb();
        }
        deferredCallbacks.clear();
    }

private:
    GLFWwindow** editorWindow;
    Logger logger;

    Xen::Scene activeScene;
    str activeSceneFile;

    char newSceneName[64]      = {'\0'};
    bool newSceneDialog        = false;
    char currentSceneName[64]  = {'\0'};
    char newGameObjectName[64] = {'\0'};
    str selectedGameObject;
    Vector<str> components = {"Behavior",
                              "Sprite Renderer",
                              "Rigidbody",
                              "Box Collider",
                              "Circle Collider",
                              "Polygon Collider",
                              "Camera",
                              "Audio Source"};
    str selectedComponent;
    const char* themes[2]  = {"XenDark.xml", "Moonlight.xml"};
    int selectedTheme      = 0;
    bool preferencesDialog = false;
    bool aboutDialog       = false;
    Vector<std::function<void()>> deferredCallbacks;

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
                        activeScene     = std::move(Xen::Scene::Load(selectedFile));
                        activeSceneFile = selectedFile;
                        UpdateWindowTitle();

                        logger.Log("Opened scene: " + activeScene.Name);
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

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Preferences")) { preferencesDialog = true; }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Pack Assets...")) {
                    deferredCallbacks.emplace_back([&]() { std::cout << "PackAssets()" << '\n'; });
                }

                ImGui::EndMenu();
            }

            // if (ImGui::BeginMenu("View")) { ImGui::EndMenu(); }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) { aboutDialog = true; }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleVar();

        if (newSceneDialog) {
            ImGui::OpenPopup("New Scene");
            if (ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Enter a name for the new scene:");
                ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                ImGui::InputText("##SceneName", newSceneName, IM_ARRAYSIZE(newSceneName));

                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    if (newSceneName[0] != '\0') {
                        activeScene     = std::move(Xen::Scene(newSceneName));
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

        if (preferencesDialog) {
            ImGui::OpenPopup("Preferences");
            ImGui::SetNextWindowSize(ImVec2(600, 400));
            if (ImGui::BeginPopupModal("Preferences", nullptr)) {
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Theme: ");
                ImGui::SameLine(80.f);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::Combo("##Themes", &selectedTheme, themes, IM_ARRAYSIZE(themes))) {
                    deferredCallbacks.emplace_back(
                      [&]() { EditorStyle::LoadAndApplyStyle(themes[selectedTheme], false); });
                }

                if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }

            if (!ImGui::IsPopupOpen("Preferences")) { preferencesDialog = false; }
        }

        if (aboutDialog) {
            ImGui::OpenPopup("About");
            if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("XEditor - Copyright © 2024, Jake Rieger");
                ImGui::Text("Version: 0.0.1-dev");
                if (ImGui::Button("OK", ImVec2(ImGui::GetColumnWidth(), 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            if (!ImGui::IsPopupOpen("About")) { aboutDialog = false; }
        }
    }

    void Toolbar() {}

    void Hierarchy() {
        ImGui::Begin("Hierarchy");

        ImGui::AlignTextToFramePadding();
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
                                                             std::move(Xen::GameObject()));
                    auto go = activeScene.GameObjects.find(newGameObjectName);
                    if (go == activeScene.GameObjects.end()) { return; }
                    auto& [name, gameObject] = *go;
                    gameObject.AddComponent("Transform");
                }

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void Inspector() {
        ImGui::Begin("Inspector");

        if (!selectedGameObject.empty()) {
            ImGui::Text("Name: %s", selectedGameObject.c_str());
            auto gameObject    = activeScene.GameObjects.find(selectedGameObject);
            auto& [goName, go] = *gameObject;

            Vector<str> componentsToRemove;
            for (auto& [compName, component] : go.Components) {
                if (compName == "Transform") {
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Transform",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        const auto transform = component->As<Xen::Transform>();

                        constexpr auto labelWidth = 80.f;
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Position:");
                        ImGui::SameLine(labelWidth);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputFloat2("##Position", &transform->X);

                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Rotation:");
                        ImGui::SameLine(labelWidth);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputFloat2("##Rotation", &transform->RotationX);

                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("Scale:");
                        ImGui::SameLine(labelWidth);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputFloat2("##Scale", &transform->ScaleX);

                        ImGui::EndChild();
                    }
                }

                if (compName == "Behavior") {
                    if (ImGui::CollapsingHeader("Behavior", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Behavior",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        ImGui::Text("Script: %s", component->As<Xen::Behavior>()->Script.c_str());
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Behavior");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Sprite Renderer") {
                    if (ImGui::CollapsingHeader("Sprite Renderer",
                                                ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Sprite Renderer",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Sprite Renderer");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Rigidbody") {
                    if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Rigidbody",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Rigidbody");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Box Collider") {
                    if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Box Collider",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Box Collider");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Circle Collider") {
                    if (ImGui::CollapsingHeader("Circle Collider",
                                                ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Circle Collider",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Circle Collider");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Polygon Collider") {
                    if (ImGui::CollapsingHeader("Polygon Collider",
                                                ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Polygon Collider",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Polygon Collider");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Camera") {
                    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Camera",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Camera");
                        }
                        ImGui::EndChild();
                    }
                }

                if (compName == "Audio Source") {
                    if (ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::BeginChild("Audio Source",
                                          ImVec2(0, 0),
                                          ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY);
                        if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 24))) {
                            componentsToRemove.emplace_back("Audio Source");
                        }
                        ImGui::EndChild();
                    }
                }
            }

            for (const auto& comp : componentsToRemove) {
                go.RemoveComponent(comp);
            }

            if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvail().x, 32))) {
                selectedComponent = "";
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
                auto usedComponents = go.GetComponentNames();
                for (const auto& component : components) {
                    if (std::ranges::any_of(usedComponents, [&](const auto& compName) {
                            return compName == component;
                        })) {
                        continue;
                    }

                    if (ImGui::Selectable(component.c_str(), component == selectedComponent)) {
                        // Update selected index
                        selectedComponent = component;
                    }
                }
                ImGui::EndChild();

                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    try {
                        go.AddComponent(selectedComponent);
                    } catch (std::runtime_error& ex) { Panic(ex.what()); }

                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
        }

        ImGui::End();
    }

    void Scene() {
        ImGui::Begin("Scene");
        ImGui::End();
    }

    void ContentBrowser() {
        ImGui::Begin("Content");
        ImGui::End();
    }

    void Messages() {
        ImGui::Begin("Messages");

        ImGui::BeginChild("Log Entries", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
        for (auto& entry : logger.Entries) {
            ImGui::Text(entry.ToString().c_str());
        }
        ImGui::EndChild();

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
        const char* saveFile = tinyfd_saveFileDialog(title, defaultFileName, 2, patterns, nullptr);

        if (saveFile) {
            activeScene.Save(saveFile);
            activeSceneFile = saveFile;
        }
    }
};