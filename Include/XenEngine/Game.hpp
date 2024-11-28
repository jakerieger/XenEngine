// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Clock.hpp"
#include "ContentManager.hpp"
#include "ScriptEngine.hpp"

// Handles window and context creation
namespace Xen {
    class IGame {
    public:
        explicit IGame(str title, int initWidth = 1280, int initHeight = 720);
        void Run(bool escToQuit = false);
        virtual ~IGame();

        virtual void LoadContent()                    = 0;
        virtual void UnloadContent()                  = 0;
        virtual void Update(const Weak<Clock>& clock) = 0;
        virtual void Draw()                           = 0;

    protected:
        GLFWwindow* mWindow = nullptr;
        Shared<Clock> mClock;
        str mTitle;
        int mInitWidth;
        int mInitHeight;
        int mCurrWidth;
        int mCurrHeight;
        bool mEscToQuit = false;
    };
}  // namespace Xen