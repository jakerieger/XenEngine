// Author: Jake Rieger
// Created: 11/21/2024.
//

#include "Game.hpp"
#include "Panic.hpp"

namespace Xen {
    static void ResizeHandler(GLFWwindow*, const int width, const int height) {
        glViewport(0, 0, width, height);
    }

    IGame::IGame(str title, int initWidth, int initHeight) {
        this->mTitle      = std::move(title);
        this->mInitWidth  = initWidth;
        this->mInitHeight = initHeight;
        this->mCurrWidth  = initWidth;
        this->mCurrHeight = initHeight;
        this->mClock      = std::make_shared<Clock>();
        // TODO: This should be read from the *.xproj file located in the project root
        this->mContentManager = std::make_shared<ContentManager>("Content");
    }

    IGame::~IGame() {
        mClock.reset();
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }

    void IGame::Run() {
        if (!glfwInit()) { Panic("Failed to initialize GLFW"); }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mWindow = glfwCreateWindow(mInitWidth, mInitHeight, mTitle.c_str(), nullptr, nullptr);
        if (!mWindow) { Panic("Failed to create GLFW window"); }

        glfwMakeContextCurrent(mWindow);
        glfwSetFramebufferSizeCallback(mWindow, ResizeHandler);

        if (!gladLoadGLLoader(RCAST<GLADloadproc>(glfwGetProcAddress))) {
            glfwTerminate();
            Panic("Failed to initialize OpenGL context");
        }

        glViewport(0, 0, mInitWidth, mInitHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
    }
}  // namespace Xen