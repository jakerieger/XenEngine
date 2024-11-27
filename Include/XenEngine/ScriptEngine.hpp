// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#include <Types.hpp>
#include <sol/sol.hpp>
#include <sol/state.hpp>

namespace Xen {
    class ScriptEngine {
    public:
        ScriptEngine(const ScriptEngine&)            = delete;
        ScriptEngine& operator=(const ScriptEngine&) = delete;

        static ScriptEngine& Get() {
            static ScriptEngine instance;
            return instance;
        }

        void Initialize() {
            mState = sol::state();
            mState.open_libraries(sol::lib::base,
                                  sol::lib::string,
                                  sol::lib::math,
                                  sol::lib::os,
                                  sol::lib::io);
            RegisterTypes();
        }

        [[nodiscard]] sol::state& GetState() {
            return mState;
        }

        template<typename... Args>
        void ExecuteFunction(const str& script, const str& name, Args&&... args) {
            mState.script_file(script);
            mState[name](std::forward<Args>(args)...);
        }

    private:
        sol::state mState;

        void RegisterTypes();

        ScriptEngine()  = default;
        ~ScriptEngine() = default;
    };
}  // namespace Xen