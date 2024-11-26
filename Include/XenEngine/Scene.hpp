// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include "ContentManager.hpp"

#include <Types.hpp>
#include <ranges>
#include <unordered_map>

#include <pugixml.hpp>
#include <Panic.hpp>
#include <sol/sol.hpp>

#include "GameObject.hpp"
#include "ScriptEngine.hpp"

namespace Xen {
    class Scene {
    public:
        str Name;
        std::unordered_map<str, GameObject> GameObjects;

        explicit Scene(str name) : Name(std::move(name)) {
            GameObjects.clear();
            // TODO: This should be read from the *.xproj file located in the project root
            this->mContentManager = std::make_shared<ContentManager>("Content");
        }

        static Unique<Scene> Load(const char* filename);

        /// @brief Saves the scene to a file on disk (*.xscene)
        void Save(const char* filename) const;
        void Awake();
        void Update(f32 dT);
        void Draw();
        void Destroy();

        Camera* GetMainCamera();

    private:
        Shared<ContentManager> mContentManager;
    };
}  // namespace Xen