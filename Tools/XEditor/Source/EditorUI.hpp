// Author: Jake Rieger
// Created: 11/10/2024.
//

#pragma once

#include <Component.hpp>
#include <GameObject.hpp>
#include <Scene.hpp>
#include <functional>
#include <GLFW/glfw3.h>

#include "EditorLog.h"
#include "EditorPreferences.hpp"
#include "EditorStyle.hpp"

#define EXIT_CODE_QUIT 69

class EditorUI {
public:
    explicit EditorUI(GLFWwindow** window);

    ~EditorUI();

    static void BeginUI();

    static void EndUI();

    void Draw();

    void DeferredCallbacks();

private:
    GLFWwindow** editorWindow;
    Logger logger;
    str activeSceneFile;
    Unique<Xen::Scene> activeScene;
    Unique<EditorPreferences> editorPreferences;

    char newSceneName[64]      = {'\0'};
    bool newSceneDialog        = false;
    char currentSceneName[64]  = {'\0'};
    char newGameObjectName[64] = {'\0'};
    str selectedGameObject;
    std::vector<str> components = {"Behavior",
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
    std::vector<std::function<void()>> deferredCallbacks;
    char xpakPath[256] = {'\0'};

private:
    void MainMenu();

    void Toolbar() {}

    void Hierarchy();

    void Inspector();

    void Scene();

    void ContentBrowser();

    void Messages();

    void UpdateWindowTitle() const;

    void SaveSceneToFile();
};