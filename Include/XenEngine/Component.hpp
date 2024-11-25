// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <glm/glm.hpp>
#include <Types.hpp>
#include <type_traits>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <sol/sol.hpp>
#include <sol/state.hpp>

namespace Xen {
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

        [[nodiscard]] inline glm::mat4 GetMatrix() const {
            // TODO: Incorporate rotation and scale transformations
            return glm::translate(glm::mat4(1), glm::vec3(X, Y, 0));
        }

        static void RegisterType(sol::state& state) {}
    };

    class Behavior final : public IComponent {
    public:
        str Script;
        Behavior() = default;
        explicit Behavior(str script) : Script(std::move(script)) {};
        [[nodiscard]] str GetScriptPath() const {
            return "Scripts/" + Script;
        }

        static void RegisterType(sol::state& state) {}
    };

    class SpriteRenderer final : public IComponent {
    public:
        explicit SpriteRenderer() : VAO(0), VBO(0) {};
        void Draw() const {}

        static void RegisterType(sol::state& state) {}

    private:
        GLuint VAO, VBO;
        void Initialize() {}
    };

    class Rigidbody final : public IComponent {
    public:
        Rigidbody() = default;

        static void RegisterType(sol::state& state) {}
    };

    class BoxCollider final : public IComponent {
    public:
        BoxCollider() = default;

        static void RegisterType(sol::state& state) {}
    };

    class CircleCollider final : public IComponent {
    public:
        CircleCollider() = default;

        static void RegisterType(sol::state& state) {}
    };

    class PolygonCollider final : public IComponent {
    public:
        PolygonCollider() = default;

        static void RegisterType(sol::state& state) {}
    };

    class Camera final : public IComponent {
    public:
        Camera() = default;

        static void RegisterType(sol::state& state) {}
    };

    class AudioSource final : public IComponent {
    public:
        AudioSource() = default;

        static void RegisterType(sol::state& state) {}
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
}  // namespace Xen