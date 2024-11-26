// Author: Jake Rieger
// Created: 11/19/2024.
//

#include "Scene.hpp"

#include "Expect.hpp"
#include "Texture.hpp"

namespace Xen {
    Unique<Scene> Scene::Load(const char* filename) {
        pugi::xml_document doc;

        const pugi::xml_parse_result result = doc.load_file(filename);
        if (!result) { Panic("Failed to parse XML file"); }

        const pugi::xml_node sceneRoot = doc.child("Scene");
        const auto sceneName           = sceneRoot.attribute("name").value();

        auto scene                 = std::make_unique<Scene>(sceneName);
        const auto& contentManager = scene->mContentManager;

        for (auto go : sceneRoot.children("GameObject")) {
            const auto goName   = go.attribute("name").value();
            const auto goActive = go.attribute("active").value() == "true";

            GameObject gameObject(goName);
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
                auto x               = ToFloat(xVal);
                auto y               = ToFloat(yVal);
                auto xRot            = ToFloat(xRotVal);
                auto yRot            = ToFloat(yRotVal);
                auto xScale          = ToFloat(xScaleVal);
                auto yScale          = ToFloat(yScaleVal);
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
                const auto sprite     = spriteRendererNode.child_value("Sprite");
                const auto loadResult = contentManager->LoadAsset(sprite);
                auto spriteAsset      = Expect(loadResult, "Failed to load sprite asset");
                const auto& component = gameObject.AddComponent("Sprite Renderer", spriteAsset);
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
            gameObject.Awake();
            scene->GameObjects.insert_or_assign(goName, std::move(gameObject));
        }

        return std::move(scene);
    }

    void Scene::Save(const char* filename) const {
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

    void Scene::Update(f32 dT) {
        for (auto& go : GameObjects | std::views::values) {
            const auto behavior = go.GetComponent<Behavior>("Behavior");
            if (behavior) {
                ScriptEngine::Instance().ExecuteFunction(behavior->GetScriptPath(),
                                                         "onUpdate",
                                                         go,
                                                         dT);
            }
        }
    }

    void Scene::Draw() {
        for (auto& go : GameObjects | std::views::values) {
            const auto spriteRenderer = go.GetComponent<SpriteRenderer>("Sprite Renderer");
            const auto transform      = go.GetComponent<Transform>("Transform");
            if (!transform) { Panic("Sprite Renderer requires Transform component."); }
            const auto camera = GetMainCamera();
            if (!camera) { Panic("Scene is missing main camera."); }
            if (spriteRenderer) {
                spriteRenderer->Draw(transform, camera->GetCamera()->As<OrthoCamera>());
            }
        }
    }

    void Scene::Destroy() {
        for (auto& go : GameObjects | std::views::values) {
            go.Destroy();
        }
    }

    Camera* Scene::GetMainCamera() {
        if (GameObjects.contains("MainCamera")) {
            return GameObjects.at("MainCamera").GetComponent<Camera>("Camera");
        }

        std::cout << "Warning: Could not find game object named 'MainCamera'. Xen will still be "
                     "able to find the main camera object, but it will be much slower."
                  << std::endl;

        for (const auto& go : GameObjects | std::views::values) {
            if (go.Components.contains("Camera")) {
                const auto& camera = go.Components.at("Camera");
                return camera->As<Camera>();
            }
        }

        return nullptr;
    }
}  // namespace Xen