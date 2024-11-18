// Author: Jake Rieger
// Created: 10/22/2024.
//

#pragma once

#include <glad/glad.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#define INC_DICTIONARY
#define INC_VECTOR
#define INC_OPTION

#include <Types/STL.h>
#include <Types/Cast.h>
#include <Types/SmartPtr.h>

#include <Panic.hpp>
#include <filesystem>
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

    class ComponentFactory {
    public:
        static Unique<IComponent> CreateComponent(const str& name) {
            if (name == "Transform") {
                return std::make_unique<Transform>();
            } else if (name == "Behavior") {
                return std::make_unique<Behavior>();
            } else if (name == "Sprite Renderer") {
                return std::make_unique<SpriteRenderer>();
            } else if (name == "Rigidbody") {
                return std::make_unique<Rigidbody>();
            } else if (name == "Box Collider") {
                return std::make_unique<BoxCollider>();
            } else if (name == "Circle Collider") {
                return std::make_unique<CircleCollider>();
            } else if (name == "Polygon Collider") {
                return std::make_unique<PolygonCollider>();
            } else if (name == "Camera") {
                return std::make_unique<Camera>();
            } else if (name == "Audio Source") {
                return std::make_unique<AudioSource>();
            }

            return nullptr;
        }
    };

    class GameObject {
    public:
        bool Active = true;
        Dictionary<str, Unique<IComponent>> Components;

        GameObject() = default;

        Unique<IComponent>& AddComponent(const str& name) {
            Components.insert_or_assign(name, ComponentFactory::CreateComponent(name));
            return Components.at(name);
        }

        void RemoveComponent(const str& name) {
            const auto it = Components.find(name);
            if (it != Components.end()) {
                // it->second.reset();
                Components.erase(it);
            }
        }

        template<typename T>
        T* GetComponent(const str& name) {
            const auto it = Components.find(name);
            if (it == Components.end()) { return nullptr; }
            return it->second->As<T>();
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
                it->second.reset();
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

        static Unique<Scene> Load(const char* filename) {
            pugi::xml_document doc;

            const pugi::xml_parse_result result = doc.load_file(filename);
            if (!result) { Panic("Failed to parse XML file"); }

            const pugi::xml_node sceneRoot = doc.child("Scene");
            const auto sceneName           = sceneRoot.attribute("name").value();

            auto scene = std::make_unique<Scene>(sceneName);

            for (auto go : sceneRoot.children("GameObject")) {
                GameObject gameObject;

                const auto goName   = go.attribute("name").value();
                const auto goActive = go.attribute("active").value() == "true";

                gameObject.Active = goActive;

                pugi::xml_node transformNode       = go.child("Transform");
                pugi::xml_node behaviorNode        = go.child("Behavior");
                pugi::xml_node spriteRendererNode  = go.child("SpriteRenderer");
                pugi::xml_node rigidbodyNode       = go.child("Rigidbody");
                pugi::xml_node boxColliderNode     = go.child("BoxCollider");
                pugi::xml_node circleColliderNode  = go.child("CircleCollider");
                pugi::xml_node polygonColliderNode = go.child("PolygonCollider");
                pugi::xml_node cameraNode          = go.child("Camera");
                pugi::xml_node audioSourceNode     = go.child("AudioSource");

                if (transformNode) {
                    const auto& component = gameObject.AddComponent("Transform");
                    const auto transform  = component->As<Transform>();
                    auto xVal             = transformNode.child("Position").attribute("x").value();
                    auto yVal             = transformNode.child("Position").attribute("y").value();
                    auto xRotVal          = transformNode.child("Rotation").attribute("x").value();
                    auto yRotVal          = transformNode.child("Rotation").attribute("y").value();
                    auto xScaleVal        = transformNode.child("Scale").attribute("x").value();
                    auto yScaleVal        = transformNode.child("Scale").attribute("y").value();
                    char* end;
                    auto x               = (f32)strtod(xVal, &end);
                    auto y               = (f32)strtod(yVal, &end);
                    auto xRot            = (f32)strtod(xRotVal, &end);
                    auto yRot            = (f32)strtod(yRotVal, &end);
                    auto xScale          = (f32)strtod(xScaleVal, &end);
                    auto yScale          = (f32)strtod(yScaleVal, &end);
                    transform->X         = x;
                    transform->Y         = y;
                    transform->RotationX = xRot;
                    transform->RotationY = yRot;
                    transform->ScaleX    = xScale;
                    transform->ScaleY    = yScale;
                }

                if (behaviorNode) {
                    const auto& component = gameObject.AddComponent("Behavior");
                    const auto behavior   = component->As<Behavior>();
                    behavior->Script      = behaviorNode.child_value("Script");
                }

                if (spriteRendererNode) {
                    const auto& component     = gameObject.AddComponent("Sprite Renderer");
                    const auto spriteRenderer = component->As<SpriteRenderer>();
                    // Do stuff
                }

                if (rigidbodyNode) {
                    const auto& component = gameObject.AddComponent("Rigidbody");
                    const auto rigidbody  = component->As<Rigidbody>();
                    // Do stuff
                }

                if (boxColliderNode) {
                    const auto& component  = gameObject.AddComponent("Box Collider");
                    const auto boxCollider = component->As<BoxCollider>();
                    // Do stuff
                }

                if (circleColliderNode) {
                    const auto& component     = gameObject.AddComponent("Circle Collider");
                    const auto circleCollider = component->As<CircleCollider>();
                    // Do stuff
                }

                if (polygonColliderNode) {
                    const auto& component      = gameObject.AddComponent("Polygon Collider");
                    const auto polygonCollider = component->As<PolygonCollider>();
                    // Do stuff
                }

                if (cameraNode) {
                    const auto& component = gameObject.AddComponent("Camera");
                    const auto camera     = component->As<Camera>();
                    // Do stuff
                }

                if (audioSourceNode) {
                    const auto& component  = gameObject.AddComponent("Audio Source");
                    const auto audioSource = component->As<AudioSource>();
                    // Do stuff
                }

                // Add to scene
                scene->GameObjects.insert_or_assign(goName, std::move(gameObject));
            }

            return std::move(scene);
        }

        /// @brief Saves the scene to a file on disk (*.xscene)
        void Save(const char* filename) const {
            pugi::xml_document doc;
            pugi::xml_node sceneRoot = doc.append_child("Scene");
            auto sceneName           = sceneRoot.append_attribute("name");
            sceneName.set_value(Name.c_str());

            for (const auto& [goName, go] : GameObjects) {
                auto goRoot   = sceneRoot.append_child("GameObject");
                auto nameAttr = goRoot.append_attribute("name");
                nameAttr.set_value(goName.c_str());
                auto activeAttr = goRoot.append_attribute("active");
                activeAttr.set_value(go.Active);

                auto& components = go.Components;
                for (auto compIter = components.begin(); compIter != components.end(); ++compIter) {
                    const auto& [name, component] = *compIter;

                    if (name == "Transform") {
                        const auto transform = component->As<Transform>();
                        auto transformRoot   = goRoot.append_child("Transform");
                        auto positionNode    = transformRoot.append_child("Position");
                        auto rotationNode    = transformRoot.append_child("Rotation");
                        auto scaleNode       = transformRoot.append_child("Scale");
                        {  // Position
                            auto xAttr = positionNode.append_attribute("x");
                            auto yAttr = positionNode.append_attribute("y");
                            xAttr.set_value(transform->X);
                            yAttr.set_value(transform->Y);
                        }
                        {  // Rotation
                            auto xAttr = rotationNode.append_attribute("x");
                            auto yAttr = rotationNode.append_attribute("y");
                            xAttr.set_value(transform->RotationX);
                            yAttr.set_value(transform->RotationY);
                        }
                        {  // Scale
                            auto xAttr = scaleNode.append_attribute("x");
                            auto yAttr = scaleNode.append_attribute("y");
                            xAttr.set_value(transform->ScaleX);
                            yAttr.set_value(transform->ScaleY);
                        }
                    } else if (name == "Behavior") {
                        const auto behavior = component->As<Behavior>();
                        auto behaviorRoot   = goRoot.append_child("Behavior");
                        auto scriptNode     = behaviorRoot.append_child("Script");
                        scriptNode.set_value(behavior->Script.c_str());
                    } else if (name == "Sprite Renderer") {
                        auto spriteRenderer     = component->As<SpriteRenderer>();
                        auto spriteRendererRoot = goRoot.append_child("SpriteRenderer");
                    } else if (name == "Rigidbody") {
                        auto rigidbody     = component->As<Rigidbody>();
                        auto rigidbodyRoot = goRoot.append_child("Rigidbody");
                    } else if (name == "Box Collider") {
                        auto boxCollider     = component->As<BoxCollider>();
                        auto boxColliderRoot = goRoot.append_child("BoxCollider");
                    } else if (name == "Circle Collider") {
                        auto circleCollider     = component->As<CircleCollider>();
                        auto circleColliderRoot = goRoot.append_child("CircleCollider");
                    } else if (name == "Polygon Collider") {
                        auto polygonCollider     = component->As<PolygonCollider>();
                        auto polygonColliderRoot = goRoot.append_child("PolygonCollider");
                    } else if (name == "Camera") {
                        auto camera     = component->As<Camera>();
                        auto cameraRoot = goRoot.append_child("Camera");
                    } else if (name == "Audio Source") {
                        auto audioSource     = component->As<AudioSource>();
                        auto audioSourceRoot = goRoot.append_child("AudioSource");
                    }
                }
            }

            if (!doc.save_file(filename)) { Panic("Failed to save Scene file"); }
        }

        void Awake(sol::state& scriptEngine) {
            for (auto& go : GameObjects | std::views::values) {
                if (auto compIter = go.Components.find("Behavior");
                    compIter != go.Components.end()) {
                    const auto& [name, component] = *compIter;
                    const auto behavior           = component->As<Behavior>();
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onAwake"](go);
                }
            }
        }

        void Update(sol::state& scriptEngine, f32 dT) {
            for (auto& go : GameObjects | std::views::values) {
                if (auto compIter = go.Components.find("Behavior");
                    compIter != go.Components.end()) {
                    const auto& [name, component] = *compIter;
                    const auto behavior           = component->As<Behavior>();
                    scriptEngine.script_file("Scripts/" + behavior->Script, sol::load_mode::text);
                    scriptEngine["onUpdate"](go, dT);
                }
            }
        }

        void Draw() {
            for (auto& go : GameObjects | std::views::values) {
                if (auto compIter = go.Components.find("Sprite Renderer");
                    compIter != go.Components.end()) {
                    const auto& [name, component] = *compIter;
                    const auto spriteRenderer     = component->As<SpriteRenderer>();
                    spriteRenderer->Draw();
                }
            }
        }

        void Destroy() {
            for (auto& go : GameObjects | std::views::values) {
                go.Destroy();
            }
        }
    };

    static void RegisterTypes(sol::state& scriptEngine) {
        scriptEngine.new_usertype<Transform>("Transform", "X", &Transform::X, "Y", &Transform::Y);

        // scriptEngine.new_usertype<GameObject>("GameObject",
        //                                       "Active",
        //                                       &GameObject::Active,
        //                                       "Components",
        //                                       &GameObject::Components);
    }

    class Asset {
    public:
        str Name;
        Vector<u8> Data;

        Asset() = default;
    };

    class ContentManager {
    public:
        ContentManager() = default;

        static Option<Asset> LoadTexture(const str& name) {
            return {};
        }

        static Option<Asset> LoadData(const str& name) {
            return {};
        }

        static Option<Asset> LoadSound(const str& name) {
            return {};
        }
    };
}  // namespace Xen