#include "XenEngine.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "Panic.hpp"

#include <sol/sol.hpp>

class IComponent {
public:
    virtual ~IComponent() = default;
};

class Transform final : public IComponent {
public:
    explicit Transform(float x = 0, float y = 0) : X(x), Y(y) {};
    float X;
    float Y;
};

class Behavior final : public IComponent {
public:
    std::string Script;
    explicit Behavior(std::string script) : Script(std::move(script)) {};
};

class GameObject {
public:
    bool Active = true;
    std::unordered_map<std::string, IComponent*> Components;

    GameObject() : Components({{}}) {}

    void Destroy() {
        for (auto it = Components.begin(); it != Components.end(); ++it) {
            delete it->second;
        }
        Components.clear();
    }
};

#include <pugixml.hpp>
#include <utility>

class Scene {
public:
    std::string Name;
    std::unordered_map<std::string, GameObject> GameObjects;

    explicit Scene(std::string name) : Name(std::move(name)) {
        GameObjects.clear();
    }

    static Scene Load(const char* filename) {
        pugi::xml_document doc;

        pugi::xml_parse_result result = doc.load_file(filename);
        if (!result) { Panic("Failed to parse XML file"); }

        const pugi::xml_node sceneRoot = doc.child("Scene");
        const auto sceneName           = sceneRoot.attribute("name").value();

        Scene scene(sceneName);

        for (auto go : sceneRoot.children("GameObject")) {
            GameObject gameObject;

            const auto goName   = go.attribute("name").value();
            const auto goActive = go.attribute("Active").value() == "true";

            gameObject.Active = goActive;

            pugi::xml_node transformNode = go.child("Transform");
            if (transformNode) {
                // Create transform component
                const auto x                       = atof(transformNode.attribute("x").value());
                const auto y                       = atof(transformNode.attribute("y").value());
                gameObject.Components["Transform"] = new Transform(x, y);
            }

            pugi::xml_node behaviorNode = go.child("Behavior");
            if (behaviorNode) {
                gameObject.Components["Behavior"] =
                  new Behavior(behaviorNode.attribute("script").value());
            }

            // Add to scene
            scene.GameObjects[goName] = gameObject;
        }

        return scene;
    }

    void Awake(sol::state& scriptEngine) {
        for (auto [name, go] : GameObjects) {
            if (go.Components["Behavior"] != nullptr) {
                auto behavior = (Behavior*)go.Components["Behavior"];
                // Load lua script
                std::cout << "Scripts/" << behavior->Script << std::endl;
                scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                scriptEngine["onAwake"](go);
            }
        }
    }

    void Update(sol::state& scriptEngine, float dT) {
        for (auto [name, go] : GameObjects) {
            if (go.Components["Behavior"] != nullptr) {
                auto behavior = (Behavior*)go.Components["Behavior"];
                // Load lua script
                scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                scriptEngine["onUpdate"](go, dT);
            }
        }
    }

    void Destroy() {
        for (auto [name, go] : GameObjects) {
            go.Destroy();
        }
    }
};

static void RegisterTypes(sol::state& scriptEngine) {
    scriptEngine.new_usertype<Transform>("Transform", "X", &Transform::X, "Y", &Transform::Y);

    scriptEngine.new_usertype<GameObject>("GameObject",
                                          "Active",
                                          &GameObject::Active,
                                          "Components",
                                          &GameObject::Components);
}

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
