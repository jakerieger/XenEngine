// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#define TYPES_ALL
#define INC_VECTOR
#define INC_OPTION
#define INC_DICTIONARY
#define INC_FUNCTION
#include <Types/Types.h>
#include <ranges>

#include "Component.hpp"

namespace Xen {
    class GameObject {
    public:
        bool Active = true;
        Dictionary<str, Unique<IComponent>> Components;

        GameObject() = default;
        Unique<IComponent>& AddComponent(const str& name);
        void RemoveComponent(const str& name);
        Vector<str> GetComponentNames();
        void Destroy();

        template<typename T>
        T* GetComponent(const str& name) {
            const auto it = Components.find(name);
            if (it == Components.end()) { return nullptr; }
            return it->second->As<T>();
        }
    };
}  // namespace Xen