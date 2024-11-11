// Author: Jake Rieger
// Created: 10/22/2024.
//

#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#define INC_DICTIONARY
#define INC_VECTOR
#include <Types/STL.h>

#include <Panic.hpp>
#include <Types/Types.h>
#include <pugixml.hpp>
#include <ranges>
#include <utility>

namespace Xen {

    class IComponent {
    public:
        virtual ~IComponent() = default;
    };

    class Transform final : public IComponent {
    public:
        explicit Transform(f32 x = 0, f32 y = 0) : X(x), Y(y) {};
        f32 X;
        f32 Y;
    };

    class Behavior final : public IComponent {
    public:
        str Script;
        explicit Behavior(str script) : Script(std::move(script)) {};
    };

    class SpriteRenderer final : public IComponent {
    public:
        SpriteRenderer() = default;
    };

    class GameObject {
    public:
        bool Active = true;
        Dictionary<str, IComponent*> Components;

        GameObject() : Components({{}}) {}

        void RemoveComponent(const str& name) {
            auto it  = Components.find(name);
            auto ptr = it->second;
            delete ptr;
            if (it != Components.end()) { Components.erase(it); }
        }

        void Destroy() {
            for (auto it = Components.begin(); it != Components.end(); ++it) {
                delete it->second;
            }
            Components.clear();
        }
    };

    class Scene {
    public:
        str Name;
        Dictionary<str, GameObject> GameObjects;

        explicit Scene(str name) : Name(std::move(name)) {
            GameObjects.clear();
        }

        static Scene Load(const char* filename) {
            pugi::xml_document doc;

            const pugi::xml_parse_result result = doc.load_file(filename);
            if (!result) { Panic("Failed to parse XML file"); }

            const pugi::xml_node sceneRoot = doc.child("Scene");
            const auto sceneName           = sceneRoot.attribute("name").value();

            Scene scene(sceneName);

            for (auto go : sceneRoot.children("GameObject")) {
                GameObject gameObject;

                const auto goName   = go.attribute("name").value();
                const auto goActive = go.attribute("active").value() == "true";

                gameObject.Active = goActive;

                pugi::xml_node transformNode = go.child("Transform");
                if (transformNode) {
                    // Create transform component
                    const auto x                       = atof(transformNode.attribute("x").value());
                    const auto y                       = atof(transformNode.attribute("y").value());
                    gameObject.Components["Transform"] = new Transform((f32)x, (f32)y);
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

        /// @brief Saves the scene to a file on disk (*.xscene)
        void Save(const char* filename) const {
            pugi::xml_document doc;
            pugi::xml_node sceneRoot = doc.append_child("Scene");
            auto sceneName           = sceneRoot.append_attribute("name");
            sceneName.set_value(Name.c_str());

            for (auto goIter = GameObjects.begin(); goIter != GameObjects.end(); ++goIter) {
                auto [goName, go] = *goIter;
                auto goRoot       = sceneRoot.append_child("GameObject");
                auto nameAttr     = goRoot.append_attribute("name");
                nameAttr.set_value(goName.c_str());
                auto activeAttr = goRoot.append_attribute("active");
                activeAttr.set_value(go.Active);

                auto components = go.Components;
                for (auto compIter = components.begin(); compIter != components.end(); ++compIter) {
                    if (compIter->first == "Transform") {
                        auto transformRoot = goRoot.append_child("Transform");
                        auto xAttr         = transformRoot.append_attribute("x");
                        xAttr.set_value(((Transform*)compIter->second)->X);
                        auto yAttr = transformRoot.append_attribute("y");
                        yAttr.set_value(((Transform*)compIter->second)->Y);
                    }

                    if (compIter->first == "Behavior") {
                        auto behaveRoot = goRoot.append_child("Behavior");
                        auto scriptAttr = behaveRoot.append_attribute("script");
                        scriptAttr.set_value(((Behavior*)compIter->second)->Script.c_str());
                    }
                }
            }

            if (!doc.save_file(filename)) { Panic("Failed to save Scene file"); }
        }

        void Awake(sol::state& scriptEngine) {
            for (auto go : GameObjects | std::views::values) {
                if (go.Components["Behavior"] != nullptr) {
                    const auto behavior = (Behavior*)go.Components["Behavior"];
                    // Load lua script
                    std::cout << "Scripts/" << behavior->Script << std::endl;
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onAwake"](go);
                }
            }
        }

        void Update(sol::state& scriptEngine, f32 dT) {
            for (auto go : GameObjects | std::views::values) {
                if (go.Components["Behavior"] != nullptr) {
                    const auto behavior = (Behavior*)go.Components["Behavior"];
                    // Load lua script
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onUpdate"](go, dT);
                }
            }
        }

        void Destroy() {
            for (auto go : GameObjects | std::views::values) {
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
}  // namespace Xen