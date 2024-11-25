// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <utility>
#include <vector>
#include <unordered_map>

#include "Component.hpp"

#include <sol/state.hpp>

namespace Xen {
    class GameObject {
    public:
        bool Active = true;
        std::unordered_map<str, Unique<IComponent>> Components;

        explicit GameObject(str name) : mName(std::move(name)) {}

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

        [[nodiscard]] str GetName() const {
            return mName;
        }

        static void RegisterType(sol::state& state) {
            state.new_usertype<GameObject>("GameObject",
                                           "Active",
                                           &GameObject::Active,
                                           "GetName",
                                           &GameObject::GetName);
        }

    private:
        str mName;
    };
}  // namespace Xen