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
#include <pugixml.hpp>
#include <Panic.hpp>
#include <sol/sol.hpp>

#include "GameObject.hpp"

namespace Xen {
    class Scene {
    public:
        str Name;
        Dictionary<str, GameObject> GameObjects;

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