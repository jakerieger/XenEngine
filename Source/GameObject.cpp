// Author: Jake Rieger
// Created: 11/19/2024.
//

#include <ranges>

#include "GameObject.hpp"
#include "ScriptEngine.hpp"

namespace Xen {
    void GameObject::RemoveComponent(const str& name) {
        const auto it = Components.find(name);
        if (it != Components.end()) { Components.erase(it); }
    }

    std::vector<str> GameObject::GetComponentNames() {
        std::vector<str> result;
        for (const auto& name : Components | std::views::keys) {
            result.push_back(name);
        }
        return result;
    }

    void GameObject::Destroy() {
        // Call user-defined code first before destroying internal game object
        Destroyed();

        for (auto it = Components.begin(); it != Components.end(); ++it) {
            it->second.reset();
        }
        Components.clear();
    }

    void GameObject::Awake() {
        const auto behavior = GetComponent<Behavior>("Behavior");
        if (!behavior) return;
        ScriptEngine::Instance().ExecuteFunction(behavior->GetScriptPath(), "onAwake", this);
    }

    void GameObject::RegisterType(sol::state& state) {
        state.new_usertype<GameObject>("GameObject",
                                       "Active",
                                       &GameObject::Active,
                                       "GetName",
                                       &GameObject::GetName,
                                       "GetTransform",
                                       &GameObject::GetTransform);
    }

    void GameObject::Destroyed() {
        const auto behavior = GetComponent<Behavior>("Behavior");
        if (!behavior) return;
        ScriptEngine::Instance().ExecuteFunction(behavior->GetScriptPath(), "onDestroyed", this);
    }
}  // namespace Xen