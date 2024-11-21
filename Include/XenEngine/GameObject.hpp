// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <vector>
#include <unordered_map>

#include "Component.hpp"

namespace Xen {
    class GameObject {
    public:
        bool Active = true;
        std::unordered_map<str, Unique<IComponent>> Components;

        GameObject() = default;
        Unique<IComponent>& AddComponent(const str& name);
        void RemoveComponent(const str& name);
        std::vector<str> GetComponentNames();
        void Destroy();

        template<typename T>
        T* GetComponent(const str& name) {
            const auto it = Components.find(name);
            if (it == Components.end()) { return nullptr; }
            return it->second->As<T>();
        }
    };
}  // namespace Xen