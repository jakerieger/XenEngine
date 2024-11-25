// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include "GameObject.hpp"

#include <sol/sol.hpp>
#include <sol/state.hpp>

namespace Xen {
    class ScriptEngine {
    public:
        ScriptEngine() {
            mState.open_libraries(sol::lib::base);
            RegisterTypes();
        }

        template<typename... Args>
        void ExecuteFunction(const str& script, const str& name, Args&&... args) {
            mState.script_file(script, sol::load_mode::text);
            mState[name](std::forward<Args>(args)...);
        }

    private:
        sol::state mState;

        void RegisterTypes() {
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
    };
}  // namespace Xen