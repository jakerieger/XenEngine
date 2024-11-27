// Author: Jake Rieger
// Created: 11/27/2024.
//

#include "ScriptEngine.hpp"
#include "GameObject.hpp"
#include "Input.hpp"
#include "Scene.hpp"

namespace Xen {
    void ScriptEngine::RegisterTypes() {
        Scene::RegisterTypes(mState);
        GameObject::RegisterType(mState);
        Transform::RegisterType(mState);
        Behavior::RegisterType(mState);
        Rigidbody::RegisterType(mState);
        SpriteRenderer::RegisterType(mState);
        BoxCollider::RegisterType(mState);
        CircleCollider::RegisterType(mState);
        PolygonCollider::RegisterType(mState);
        AudioSource::RegisterType(mState);
        Camera::RegisterType(mState);
    }
}  // namespace Xen