// Author: Jake Rieger
// Created: 11/19/2024.
//

#include "GameObject.hpp"

namespace Xen {
    Unique<IComponent>& GameObject::AddComponent(const str& name) {
        Components.insert_or_assign(name, ComponentFactory::CreateComponent(name));
        return Components.at(name);
    }

    void GameObject::RemoveComponent(const str& name) {
        const auto it = Components.find(name);
        if (it != Components.end()) {
            // it->second.reset();
            Components.erase(it);
        }
    }

    Vector<str> GameObject::GetComponentNames() {
        Vector<str> result;
        for (const auto& name : Components | std::views::keys) {
            result.push_back(name);
        }
        return result;
    }

    void GameObject::Destroy() {
        for (auto it = Components.begin(); it != Components.end(); ++it) {
            it->second.reset();
        }
        Components.clear();
    }
}  // namespace Xen