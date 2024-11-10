// Author: Jake Rieger
// Created: 11/9/2024.
//

#include "EditorWindow.hpp"
#include "XenEngine.hpp"

using namespace XEditor;
using namespace Xen;

int main() {
    Scene testScene("TestScene");

    GameObject player;
    player.Active = false;

    auto transform = new Transform();
    transform->X   = 10.4;
    transform->Y   = 77.2;
    player.Components.insert_or_assign("Transform", transform);

    auto behavior = new Behavior("Player.lua");
    player.Components.insert_or_assign("Behavior", behavior);

    testScene.GameObjects.insert_or_assign("Player", player);
    testScene.Save("TestScene.xscene");

    // const EditorWindow editor(1300, 760);
    // editor.Run();
    return 0;
}