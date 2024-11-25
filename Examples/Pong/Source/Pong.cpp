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
        ScriptEngine::Instance().Initialize();
        mMainScene = Scene::Load("Scenes/Main.xscene");

        auto shader =
          std::make_shared<Shader>(Shaders::SpriteShader::Vertex, Shaders::SpriteShader::Fragment);
        auto camera = CreateCamera<OrthoCamera>(1280, 720);
    }

    void UnloadContent() override {
        mMainScene.reset();
    }

    void Update(const Weak<Clock>& clock) override {
        const auto dT = clock.lock()->GetDeltaTime();
        mMainScene->Update(dT);
    }

    void Draw() override {
        mMainScene->Awake();
    }

private:
    Unique<Scene> mMainScene;
};

int main() {
    Pong game;
    game.Run();
    return 0;
}