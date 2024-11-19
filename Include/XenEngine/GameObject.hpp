// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#define TYPES_ALL
#define INC_VECTOR
#define INC_OPTION
#define INC_DICTIONARY
#include <Types/Types.h>
#include <ranges>

#include "Component.hpp"

namespace Xen {
    class GameObject {
    public:
        bool Active = true;
        Dictionary<str, Unique<IComponent>> Components;

        GameObject() = default;

        Unique<IComponent>& AddComponent(const str& name) {
            Components.insert_or_assign(name, ComponentFactory::CreateComponent(name));
            return Components.at(name);
        }

        void RemoveComponent(const str& name) {
            const auto it = Components.find(name);
            if (it != Components.end()) {
                // it->second.reset();
                Components.erase(it);
            }
        }

        template<typename T>
        T* GetComponent(const str& name) {
            const auto it = Components.find(name);
            if (it == Components.end()) { return nullptr; }
            return it->second->As<T>();
        }

        Vector<str> GetComponentNames() {
            Vector<str> result;
            for (const auto& name : Components | std::views::keys) {
                result.push_back(name);
            }
            return result;
        }

        void Destroy() {
            for (auto it = Components.begin(); it != Components.end(); ++it) {
                it->second.reset();
            }
            Components.clear();
        }
    };
}  // namespace Xen