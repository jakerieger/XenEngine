// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <ContentManager.hpp>

using namespace Xen;

int main() {
    const auto contentManager = std::make_unique<ContentManager>("Content");

    auto result     = contentManager->LoadAsset("sprites/ball");
    const auto ball = Expect(result, "Failed to load ball texture");

    result            = contentManager->LoadAsset("sprites/paddle");
    const auto paddle = Expect(result, "Failed to load paddle texture");

    result             = contentManager->LoadAsset("audio/bg_music");
    const auto bgMusic = Expect(result, "Failed to load bg music");

    return 0;
}