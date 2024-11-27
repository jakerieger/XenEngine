// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <utility>
#include <vector>
#include <unordered_map>
#include <sol/state.hpp>

#include "Component.hpp"

namespace Xen {
    class GameObject {
    public:
        bool Active = true;
        std::unordered_map<str, Unique<IComponent>> Components;

        explicit GameObject(str name) : mName(std::move(name)) {}

        void RemoveComponent(const str& name);
        std::vector<str> GetComponentNames();
        void Destroy();

        template<typename T>
        T* GetComponentAs(const str& name) {
            const auto it = Components.find(name);
            if (it == Components.end()) { return nullptr; }
            return it->second->As<T>();
        }

        IComponent* GetComponent(const str& name) {
            if (!Components.contains(name)) { return nullptr; }
            return Components[name].get();
        }

        template<typename... Args>
        Unique<IComponent>& AddComponent(const str& name, Args&&... args) {
            Components.insert_or_assign(
              name,
              ComponentFactory::CreateComponent(name, std::forward<Args>(args)...));
            return Components.at(name);
        }

        [[nodiscard]] str GetName() const {
            return mName;
        }

        [[nodiscard]] Transform* GetTransform() {
            return GetComponentAs<Transform>("Transform");
        }

        void Awake();

        static void RegisterType(sol::state& state);

    private:
        str mName;
        void Destroyed();
    };
}  // namespace Xen