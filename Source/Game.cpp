// Author: Jake Rieger
// Created: 11/21/2024.
//

#include <Panic.hpp>

#include "Game.hpp"
#include "Input.hpp"

namespace Xen {
    static bool gEscToQuit = false;

    static void ResizeHandler(GLFWwindow*, const int width, const int height) {
        glViewport(0, 0, width, height);
    }

    static void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (gEscToQuit) {
            if (key == KeyCode::Escape) { glfwSetWindowShouldClose(window, true); }
        }

        if (action == GLFW_PRESS) {
            Input::Get().UpdateKeyState(key, true);
        } else if (action == GLFW_RELEASE) {
            Input::Get().UpdateKeyState(key, false);
        }
    }

    static void MouseButtonHandler(GLFWwindow*, int button, int action, int mods) {
        if (action == GLFW_PRESS) {
            Input::Get().UpdateMouseButtonState(button, true);
        } else if (action == GLFW_RELEASE) {
            Input::Get().UpdateMouseButtonState(button, false);
        }
    }

    static void CursorPosHandler(GLFWwindow*, double x, double y) {
        Input::Get().UpdateMousePosition((i32)x, (i32)y);
    }

    IGame::IGame(str title, int initWidth, int initHeight) {
        this->mTitle      = std::move(title);
        this->mInitWidth  = initWidth;
        this->mInitHeight = initHeight;
        this->mCurrWidth  = initWidth;
        this->mCurrHeight = initHeight;
        this->mClock      = std::make_shared<Clock>();
        ScriptEngine::Get().Initialize();
        Input::Get().RegisterGlobals(ScriptEngine::Get().GetState());
    }

    IGame::~IGame() {
        mClock.reset();
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void IGame::Run(bool escToQuit) {
        if (!glfwInit()) { Panic("Failed to initialize GLFW"); }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mWindow = glfwCreateWindow(mInitWidth, mInitHeight, mTitle.c_str(), nullptr, nullptr);
        if (!mWindow) { Panic("Failed to create GLFW window"); }

        gEscToQuit = escToQuit;

        glfwMakeContextCurrent(mWindow);
        glfwSetFramebufferSizeCallback(mWindow, ResizeHandler);
        glfwSetKeyCallback(mWindow, KeyHandler);
        glfwSetMouseButtonCallback(mWindow, MouseButtonHandler);
        glfwSetCursorPosCallback(mWindow, CursorPosHandler);

        if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
            glfwTerminate();
            Panic("Failed to initialize OpenGL context");
        }

        glViewport(0, 0, mInitWidth, mInitHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // ====================================================================================== //
        //        THE ENTIRE GAME'S LIFECYCLE IS CONTAINED IN THE FOLLOWING LINES OF CODE         //
        // ====================================================================================== //
        LoadContent();
        mClock->Start();
        while (!glfwWindowShouldClose(mWindow)) {
            mClock->Tick();
            {
                glfwPollEvents();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                Update(mClock);
                Draw();

                glfwSwapBuffers(mWindow);
            }
            mClock->Update();
        }
        mClock->Stop();
        UnloadContent();
        // ====================================================================================== //
        // -------------------------------------------------------------------------------------- //
        // ====================================================================================== //
    }
}  // namespace Xen