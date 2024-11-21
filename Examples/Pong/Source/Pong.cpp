// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <ContentManager.hpp>
#include <Game.hpp>

using namespace Xen;

class Pong final : public IGame {
public:
    Pong() : IGame("Pong") {}

    void CreateGLResources() override {}

    void DestroyGLResources() override {}

    void Update(const Weak<Clock>& clock) override {
        const auto dT = clock.lock()->GetDeltaTime();
        std::cout << 1.f / dT << std::endl;
    }

    void Draw() override {}
};

int main() {
    Pong game;
    game.Run();
    return 0;
}