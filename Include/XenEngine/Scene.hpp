// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <ranges>
#include <unordered_map>

#include <pugixml.hpp>
#include <Panic.hpp>
#include <sol/sol.hpp>

#include "GameObject.hpp"

namespace Xen {
    class Scene {
    public:
        str Name;
        std::unordered_map<str, GameObject> GameObjects;

        explicit Scene(str name) : Name(std::move(name)) {
            GameObjects.clear();
        }

        static Unique<Scene> Load(const char* filename);

        /// @brief Saves the scene to a file on disk (*.xscene)
        void Save(const char* filename) const;

        void Awake(sol::state& scriptEngine);

        void Update(sol::state& scriptEngine, f32 dT);

        void Draw();

        void Destroy();
    };
}  // namespace Xen