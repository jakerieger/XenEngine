#include "XenEngine.hpp"

using namespace Xen;

int main() {
    sol::state scriptEngine;
    scriptEngine.open_libraries(sol::lib::base);
    RegisterTypes(scriptEngine);

    auto scene = Scene::Load("Scenes/Main.xscene");
    scene.Awake(scriptEngine);

    // for (;;) {
    //     scene.Update(scriptEngine, 0.011);
    // }

    scene.Destroy();

    return 0;
}
