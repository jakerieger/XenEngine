// Author: Jake Rieger
// Created: 11/14/2024.
//

#include "Camera.hpp"
#include "CommonShaders.hpp"
#include "Scene.hpp"
#include "Shader.hpp"

#include <Expect.hpp>
#include <Texture.hpp>
#include <ContentManager.hpp>
#include <Game.hpp>

using namespace Xen;

class Pong final : public IGame {
public:
    Pong() : IGame("Pong") {}

    void LoadContent() override {
        mMainScene                                     = Scene::Load("Scenes/Main.xscene");
        ScriptEngine::Get().GetState()["SceneManager"] = mMainScene.get();
    }

    void UnloadContent() override {
        mMainScene->Destroy();
        mMainScene.reset();
    }

    void Update(const Weak<Clock>& clock) override {
        const auto dT = clock.lock()->GetDeltaTime();
        mMainScene->Update(dT);
    }

    void Draw() override {
        mMainScene->Draw();
    }

private:
    Unique<Scene> mMainScene;
};

int main() {
    Pong game;
    game.Run();
    return 0;
}