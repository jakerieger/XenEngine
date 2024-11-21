// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <Expect.hpp>
#include <Texture.hpp>
#include <ContentManager.hpp>
#include <Game.hpp>

using namespace Xen;

class Pong final : public IGame {
public:
    Pong() : IGame("Pong") {}

    void LoadContent() override {
        auto result     = mContentManager->LoadAsset("sprites/ball");
        const auto ball = Expect(result, "Failed to load ball asset");
        mBallTexture    = CAST<i32>(Texture::LoadFromMemory(ball.Data, 48, 48));

        result            = mContentManager->LoadAsset("sprites/paddle");
        const auto paddle = Expect(result, "Failed to load paddle asset");
        mPaddleTexture    = CAST<i32>(Texture::LoadFromMemory(paddle.Data, 24, 200));
    }

    void UnloadContent() override {
        Texture::Delete(mBallTexture);
        Texture::Delete(mPaddleTexture);
    }

    void Update(const Weak<Clock>& clock) override {
        const auto dT = clock.lock()->GetDeltaTime();
    }

    void Draw() override {}

private:
    i32 mBallTexture   = -1;
    i32 mPaddleTexture = -1;
};

int main() {
    Pong game;
    game.Run();
    return 0;
}