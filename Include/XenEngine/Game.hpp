// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#define TYPES_ALL
#include <Types/Types.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Clock.hpp"
#include "ContentManager.hpp"

// Handles window and context creation
namespace Xen {
    class IGame {
    public:
        explicit IGame(str title, int initWidth = 1280, int initHeight = 720);
        void Run();
        virtual ~IGame();

        virtual void CreateGLResources()              = 0;
        virtual void DestroyGLResources()             = 0;
        virtual void Update(const Weak<Clock>& clock) = 0;
        virtual void Draw()                           = 0;

    protected:
        GLFWwindow* mWindow = nullptr;
        Shared<Clock> mClock;
        Shared<ContentManager> mContentManager;
        str mTitle;
        int mInitWidth;
        int mInitHeight;
        int mCurrWidth;
        int mCurrHeight;
    };

    static Shared<IGame> ActiveGame = nullptr;
}  // namespace Xen