// Author: Jake Rieger
// Created: 11/19/2024.
//

#include "GameObject.hpp"
#include <ranges>

namespace Xen {
    void GameObject::RemoveComponent(const str& name) {
        const auto it = Components.find(name);
        if (it != Components.end()) {
            // it->second.reset();
            Components.erase(it);
        }
    }

    std::vector<str> GameObject::GetComponentNames() {
        std::vector<str> result;
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