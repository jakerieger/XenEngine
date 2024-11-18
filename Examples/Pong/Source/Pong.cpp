// Author: Jake Rieger
// Created: 11/14/2024.
//

#include <XenEngine.hpp>

using namespace Xen;

int main() {
    const auto ballTexture =
      Expect(ContentManager::LoadTexture("sprites/ball"), "Failed to get ball sprite.");
    const auto paddleTexture =
      Expect(ContentManager::LoadTexture("sprites/paddle"), "Failed to get paddle sprite.");
    const auto bgMusic =
      Expect(ContentManager::LoadSound("audio/bg_music"), "Failed to get background music audio.");

    return 0;
}