// Author: Jake Rieger
// Created: 11/14/2024.
//

#include "Scene.hpp"

#include <Expect.hpp>
#include <Texture.hpp>
#include <ContentManager.hpp>
#include <Game.hpp>

using namespace Xen;

class Pong final : public IGame {
public:
    Pong() : IGame("Pong") {}

    void LoadContent() override {
        mMainScene = Scene::Load("Scenes/Main.xscene");
    }

    void UnloadContent() override {

    }

    void Update(const Weak<Clock>& clock) override {
        const auto dT = clock.lock()->GetDeltaTime();
        // mMainScene->Update({}, dT);
    }

    void Draw() override {}

private:
    Unique<Scene> mMainScene;
};

int main() {
    Pong game;
    game.Run();
    return 0;
}