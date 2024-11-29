// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once
#pragma warning(disable : 4244)
#pragma warning(disable : 4096)

#include "Shader.hpp"
#include "CommonShaders.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "Camera.hpp"
#include "ContentManager.hpp"
#include "Primitives.hpp"

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
        Transform() : X(0), Y(0), RotationX(0), RotationY(0), ScaleX(1), ScaleY(1) {};
        explicit Transform(f32 x, f32 y)
            : X(x), Y(y), RotationX(0), RotationY(0), ScaleX(1), ScaleY(1) {};
        f32 X;
        f32 Y;
        f32 RotationX;
        f32 RotationY;
        f32 ScaleX;
        f32 ScaleY;

        [[nodiscard]] inline glm::mat4 GetMatrix() const {
            auto mat = glm::mat4(1.0f);
            mat      = glm::scale(mat, glm::vec3(ScaleX, ScaleY, 1.0f));
            mat      = glm::rotate(mat, glm::radians(RotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            mat      = glm::rotate(mat, glm::radians(RotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            mat      = glm::translate(mat, glm::vec3(X, Y, 1.0f));
            return mat;
        }

        inline void Translate(f32 x, f32 y) {
            X += x;
            Y += y;
        }

        inline void Scale(f32 x, f32 y) {
            ScaleX *= x;
            ScaleY *= y;
        }

        inline void Rotate(f32 x, f32 y) {
            RotationX += x;
            RotationY += y;
        }

        inline void SetPosition(f32 x, f32 y) {
            X = x;
            Y = y;
        }

        inline void SetRotation(f32 x, f32 y) {
            RotationX = x;
            RotationY = y;
        }

        inline void SetScale(f32 x, f32 y) {
            ScaleX = x;
            ScaleY = y;
        }

        static void RegisterType(sol::state& state) {
            state.new_usertype<Transform>("Transform",
                                          "X",
                                          &Transform::X,
                                          "Y",
                                          &Transform::Y,
                                          "RotationX",
                                          &Transform::RotationX,
                                          "RotationY",
                                          &Transform::RotationY,
                                          "ScaleX",
                                          &Transform::ScaleX,
                                          "ScaleY",
                                          &Transform::ScaleY,
                                          "Translate",
                                          &Transform::Translate,
                                          "Rotate",
                                          &Transform::Rotate,
                                          "Scale",
                                          &Transform::Scale,
                                          "SetPosition",
                                          &Transform::SetPosition,
                                          "SetRotation",
                                          &Transform::SetRotation,
                                          "SetScale",
                                          &Transform::SetScale);
        }
    };

    class Behavior final : public IComponent {
    public:
        str Script;
        Behavior() = default;
        explicit Behavior(str script) : Script(std::move(script)) {};
        [[nodiscard]] str GetScriptPath() const {
            return "Scripts/" + Script;
        }

        static void RegisterType(sol::state& state) {
            state.new_usertype<Behavior>("Behavior", "Script", &Behavior::Script);
        }
    };

    class SpriteRenderer final : public IComponent {
    public:
        SpriteRenderer() : mTexture(0) {};
        explicit SpriteRenderer(const Shared<Asset>& spriteAsset) : mTexture(0) {
            Initialize(spriteAsset);
        }

        ~SpriteRenderer() override {
            mShader.reset();
            mVAO.reset();
            Texture::Delete(mTexture);
        }

        void Draw(const Transform* transform, const OrthoCamera* camera) const;

        static void RegisterType(sol::state& state) {}

    private:
        Unique<VertexArray> mVAO;
        Unique<Shader> mShader;
        u32 mTexture;

        void Initialize(const Shared<Asset>& spriteAsset) {
            if (!spriteAsset->Metadata.contains("width") ||
                !spriteAsset->Metadata.contains("height")) {
                std::cerr << "ERROR: Invalid sprite metadata (missing 'width'/'height' properties)."
                          << std::endl;
                return;
            }

            const auto data   = spriteAsset->Data;
            const auto width  = ToInt(spriteAsset->Metadata.at("width"));
            const auto height = ToInt(spriteAsset->Metadata.at("height"));
            mTexture          = Texture::LoadFromMemory(data, width, height);
            mShader           = std::make_unique<Shader>(Shaders::SpriteShader::Vertex,
                                               Shaders::SpriteShader::Fragment);
            mVAO              = std::make_unique<VertexArray>();
            std::vector<VertexAttribute> attributes = {
              {"aVertex", 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)nullptr},
            };
            mVAO->Bind();
            mVAO->CreateVertexBuffer<f32>(Primitives::QuadVertTex, attributes);
            VertexArray::Unbind();
        }
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
        Camera() {
            mCamera = CreateCamera<OrthoCamera>(1280, 720);
        }

        [[nodiscard]] ICamera* GetCamera() const {
            return mCamera.get();
        }

        static void RegisterType(sol::state& state) {
            state.new_usertype<Camera>("Camera");
        }

    private:
        Shared<ICamera> mCamera;
    };

    class AudioSource final : public IComponent {
    public:
        AudioSource() = default;

        static void RegisterType(sol::state& state) {}
    };

    class ComponentFactory {
    public:
        template<typename... Args>
        static Unique<IComponent> CreateComponent(const str& name, Args&&... args) {
            if (name == "Transform") {
                return std::make_unique<Transform>();
            } else if (name == "Behavior") {
                return std::make_unique<Behavior>();
            } else if (name == "Sprite Renderer") {
                return std::make_unique<SpriteRenderer>(std::forward<Args>(args)...);
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

    inline void SpriteRenderer::Draw(const Transform* transform, const OrthoCamera* camera) const {
        mShader->Bind();
        mShader->SetInt("uSprite", 0);
        const auto model = transform->GetMatrix();
        const auto mvp   = camera->GetViewProjection() * model;
        mShader->SetMat4("uMVP", mvp);
        Texture::Bind(mTexture, 0);
        mVAO->Bind();
        mVAO->Draw(GL_TRIANGLE_STRIP);
        VertexArray::Unbind();
        Texture::Unbind();
        Shader::Unbind();
    }
}  // namespace Xen