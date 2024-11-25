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
#include "ScriptEngine.hpp"

namespace Xen {
    class Scene {
    public:
        str Name;
        std::unordered_map<str, GameObject> GameObjects;

        explicit Scene(str name, const Weak<ScriptEngine>& scriptEngine) : Name(std::move(name)) {
            mScriptEngine = scriptEngine;
            GameObjects.clear();
        }

        static Unique<Scene> Load(const char* filename, const Weak<ScriptEngine>& scriptEngine);

        /// @brief Saves the scene to a file on disk (*.xscene)
        void Save(const char* filename) const;

        void Awake();

        void Update(f32 dT);

        void Draw();

        void Destroy();

        Camera* GetMainCamera();

    private:
        Weak<ScriptEngine> mScriptEngine;
    };
}  // namespace Xen