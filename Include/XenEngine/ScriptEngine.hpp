// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include "GameObject.hpp"
#include "InputCodes.hpp"

#include <sol/sol.hpp>
#include <sol/state.hpp>

static bool GetKey(int key) {
    return false;
}

namespace Xen {
    class ScriptEngine {
    public:
        ScriptEngine(const ScriptEngine&)            = delete;
        ScriptEngine& operator=(const ScriptEngine&) = delete;

        static ScriptEngine& Instance() {
            static ScriptEngine instance;
            return instance;
        }

        void Initialize() {
            mState = sol::state();
            mState.open_libraries(sol::lib::base,
                                  sol::lib::string,
                                  sol::lib::math,
                                  sol::lib::os,
                                  sol::lib::io);
            RegisterGlobals();
            RegisterTypes();
        }

        template<typename... Args>
        void ExecuteFunction(const str& script, const str& name, Args&&... args) {
            mState.script_file(script);
            mState[name](std::forward<Args>(args)...);
        }

    private:
        sol::state mState;

        void RegisterGlobals() {
            mState["KEY_W"]     = KeyCode::W;
            mState["KEY_S"]     = KeyCode::S;
            mState["KEY_A"]     = KeyCode::A;
            mState["KEY_D"]     = KeyCode::D;
            mState["KEY_SPACE"] = KeyCode::Space;
            mState["KEY_UP"]    = KeyCode::Up;
            mState["KEY_DOWN"]  = KeyCode::Down;
            mState["KEY_LEFT"]  = KeyCode::Left;
            mState["KEY_RIGHT"] = KeyCode::Right;

            // TODO: Work on input system
            mState["GetKey"] = &GetKey;
        }

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

        ScriptEngine()  = default;
        ~ScriptEngine() = default;
    };
}  // namespace Xen