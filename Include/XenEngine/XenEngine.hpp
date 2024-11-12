// Author: Jake Rieger
// Created: 10/22/2024.
//

#pragma once

#include <glad/glad.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#define INC_DICTIONARY
#define INC_VECTOR

#include <Types/STL.h>
#include <Types/Cast.h>
#include <Types/SmartPtr.h>

#include <Panic.hpp>
#include <Types/Types.h>
#include <pugixml.hpp>
#include <ranges>
#include <typeindex>
#include <utility>

namespace Xen {
    namespace OpenGL {
        class Shader {};

        class Texture {
        public:
            explicit Texture(const char* filename) : id(0) {
                LoadFromFile(filename);
            }

            ~Texture() {
                glDeleteTextures(1, &id);
            }

            void Bind() const {
                glBindTexture(GL_TEXTURE_2D, id);
            }

        private:
            GLuint id;
            void LoadFromFile(const char* filename) {
                // Pull code for this from NuggetGame or similar
            }
        };
    }  // namespace OpenGL

    class IComponent {
    public:
        virtual ~IComponent() = default;

        template<typename T>
            requires std::is_base_of_v<IComponent, T>
        T* As() {
            return DCAST<T*>(this);
        }
    };

    class Transform final : public IComponent {
    public:
        explicit Transform(f32 x = 0, f32 y = 0)
            : X(x), Y(y), RotationX(0), RotationY(0), ScaleX(1), ScaleY(1) {};
        f32 X;
        f32 Y;
        f32 RotationX;
        f32 RotationY;
        f32 ScaleX;
        f32 ScaleY;
    };

    class Behavior final : public IComponent {
    public:
        str Script;
        Behavior() = default;
        explicit Behavior(str script) : Script(std::move(script)) {};
    };

    class SpriteRenderer final : public IComponent {
    public:
        explicit SpriteRenderer() : shader(nullptr), texture({}), VAO(0), VBO(0) {};
        void Draw() const {}

    private:
        Unique<OpenGL::Shader> shader;
        OpenGL::Texture texture;
        GLuint VAO, VBO;
        void Initialize() {}
    };

    class Rigidbody final : public IComponent {
    public:
        Rigidbody() = default;
    };

    class BoxCollider final : public IComponent {
    public:
        BoxCollider() = default;
    };

    class CircleCollider final : public IComponent {
    public:
        CircleCollider() = default;
    };

    class PolygonCollider final : public IComponent {
    public:
        PolygonCollider() = default;
    };

    class Camera final : public IComponent {
    public:
        Camera() = default;
    };

    class AudioSource final : public IComponent {
    public:
        AudioSource() = default;
    };

    // TODO: I may or may not keep this... idk yet
    static const Dictionary<str, std::function<IComponent*()>> ComponentFactory = {
      {"Transform", []() { return new Transform(); }},
      {"Behavior", []() { return new Behavior(); }},
      {"Sprite Renderer", []() { return new SpriteRenderer(); }},
      {"Rigidbody", []() { return new Rigidbody(); }},
      {"Box Collider", []() { return new BoxCollider(); }},
      {"Circle Collider", []() { return new CircleCollider(); }},
      {"Polygon Collider", []() { return new PolygonCollider(); }},
      {"Camera", []() { return new Camera(); }},
      {"Audio Source", []() { return new AudioSource(); }},
    };

    class GameObject {
    public:
        bool Active = true;
        Dictionary<str, IComponent*> Components;

        GameObject() : Components({{}}) {}

        void AddComponent(const str& name) {
            const auto componentFactory = ComponentFactory.find(name);
            if (componentFactory == ComponentFactory.end()) {
                throw std::runtime_error("Component not found");
            }
            const auto constructor = componentFactory->second;
            const auto component   = constructor();
            Components.insert_or_assign(name, component);
        }

        void RemoveComponent(const str& name) {
            const auto it  = Components.find(name);
            const auto ptr = it->second;
            // All components **SHOULD** get cleaned up here.
            // I still need to implement memory tracking so the dev can
            // monitor memory usage in the editor and check for leaks.
            delete ptr;
            if (it != Components.end()) { Components.erase(it); }
        }

        Vector<str> GetComponentNames() {
            Vector<str> result;
            for (const auto& name : Components | std::views::keys) {
                result.push_back(name);
            }
            return result;
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
                    char* end;
                    const auto x = strtod(transformNode.attribute("x").value(), &end);
                    const auto y = strtod(transformNode.attribute("y").value(), &end);
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
                        scriptAttr.set_value(compIter->second->As<Behavior>()->Script.c_str());
                    }
                }
            }

            if (!doc.save_file(filename)) { Panic("Failed to save Scene file"); }
        }

        void Awake(sol::state& scriptEngine) {
            for (auto go : GameObjects | std::views::values) {
                if (go.Components["Behavior"] != nullptr) {
                    const auto behavior = go.Components["Behavior"]->As<Behavior>();
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onAwake"](go);
                }
            }
        }

        void Update(sol::state& scriptEngine, f32 dT) {
            for (auto go : GameObjects | std::views::values) {
                if (go.Components["Behavior"] != nullptr) {
                    const auto behavior = go.Components["Behavior"]->As<Behavior>();
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onUpdate"](go, dT);
                }
            }
        }

        void Draw(sol::state& scriptEngine) {
            for (auto go : GameObjects | std::views::values) {
                if (go.Components["Sprite Renderer"] != nullptr) {
                    const auto renderer = go.Components["Sprite Renderer"]->As<SpriteRenderer>();
                    renderer->Draw();
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