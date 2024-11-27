// Author: Jake Rieger
// Created: 11/26/2024.
//

#pragma once

#include "InputCodes.hpp"
#include <sol/state_view.hpp>

namespace Xen {
    class Input {
    public:
        Input(const Input&)            = delete;
        Input& operator=(const Input&) = delete;

        static Input& Get() {
            static Input instance;
            return instance;
        }

        bool GetKeyDown(i32 key) {
            return mKeyStates[key].Pressed;
        }

        bool GetKeyUp(i32 key) {
            return mKeyStates[key].Released;
        }

        bool GetMouseButtonDown(i32 button) {
            return mMouseStates[button].Pressed;
        }

        bool GetMouseButtonUp(i32 button) {
            return mMouseStates[button].Released;
        }

        [[nodiscard]] i32 GetMouseX() const {
            return mMouseX;
        }

        [[nodiscard]] i32 GetMouseY() const {
            return mMouseY;
        }

        void RegisterGlobals(sol::state_view& sv) {
            sv["KEY_A"] = KeyCode::A;
            sv["KEY_B"] = KeyCode::B;
            sv["KEY_C"] = KeyCode::C;
            sv["KEY_D"] = KeyCode::D;
            sv["KEY_E"] = KeyCode::E;
            sv["KEY_F"] = KeyCode::F;
            sv["KEY_G"] = KeyCode::G;
            sv["KEY_H"] = KeyCode::H;
            sv["KEY_I"] = KeyCode::I;
            sv["KEY_J"] = KeyCode::J;
            sv["KEY_K"] = KeyCode::K;
            sv["KEY_L"] = KeyCode::L;
            sv["KEY_M"] = KeyCode::M;
            sv["KEY_N"] = KeyCode::N;
            sv["KEY_O"] = KeyCode::O;
            sv["KEY_P"] = KeyCode::P;
            sv["KEY_Q"] = KeyCode::Q;
            sv["KEY_R"] = KeyCode::R;
            sv["KEY_S"] = KeyCode::S;
            sv["KEY_T"] = KeyCode::T;
            sv["KEY_U"] = KeyCode::U;
            sv["KEY_V"] = KeyCode::V;
            sv["KEY_W"] = KeyCode::W;
            sv["KEY_X"] = KeyCode::X;
            sv["KEY_Y"] = KeyCode::Y;
            sv["KEY_Z"] = KeyCode::Z;

            sv["KEY_1"] = KeyCode::One;
            sv["KEY_2"] = KeyCode::Two;
            sv["KEY_3"] = KeyCode::Three;
            sv["KEY_4"] = KeyCode::Four;
            sv["KEY_5"] = KeyCode::Five;
            sv["KEY_6"] = KeyCode::Six;
            sv["KEY_7"] = KeyCode::Seven;
            sv["KEY_8"] = KeyCode::Eight;
            sv["KEY_9"] = KeyCode::Nine;
            sv["KEY_0"] = KeyCode::Zero;

            sv["KEY_MINUS"]     = KeyCode::Minus;
            sv["KEY_EQUAL"]     = KeyCode::Equal;
            sv["KEY_BACKSPACE"] = KeyCode::Backspace;
            sv["KEY_TAB"]       = KeyCode::Tab;
            sv["KEY_HOME"]      = KeyCode::Home;
            sv["KEY_LEFT"]      = KeyCode::Left;
            sv["KEY_UP"]        = KeyCode::Up;
            sv["KEY_RIGHT"]     = KeyCode::Right;
            sv["KEY_DOWN"]      = KeyCode::Down;
            sv["KEY_ESCAPE"]    = KeyCode::Escape;
            sv["KEY_ENTER"]     = KeyCode::Enter;
            sv["KEY_SPACE"]     = KeyCode::Space;

            sv["KEY_F1"]  = KeyCode::F1;
            sv["KEY_F2"]  = KeyCode::F2;
            sv["KEY_F3"]  = KeyCode::F3;
            sv["KEY_F4"]  = KeyCode::F4;
            sv["KEY_F5"]  = KeyCode::F5;
            sv["KEY_F6"]  = KeyCode::F6;
            sv["KEY_F7"]  = KeyCode::F7;
            sv["KEY_F8"]  = KeyCode::F8;
            sv["KEY_F9"]  = KeyCode::F9;
            sv["KEY_F10"] = KeyCode::F10;
            sv["KEY_F11"] = KeyCode::F11;
            sv["KEY_F12"] = KeyCode::F12;

            sv["MOUSE_LEFT"]   = MouseButton::Left;
            sv["MOUSE_RIGHT"]  = MouseButton::Right;
            sv["MOUSE_MIDDLE"] = MouseButton::Middle;

            sv.new_usertype<Input>("Input",
                                   "GetKeyDown",
                                   &Input::GetKeyDown,
                                   "GetKeyUp",
                                   &Input::GetKeyUp,
                                   "GetMouseButtonDown",
                                   &Input::GetMouseButtonDown,
                                   "GetMouseButtonUp",
                                   &Input::GetMouseButtonUp,
                                   "GetMouseX",
                                   &Input::GetMouseX,
                                   "GetMouseY",
                                   &Input::GetMouseY);

            // Provide a global accessor to this instance in Lua
            sv["InputManager"] = this;
        }

        void UpdateKeyState(int key, bool pressed) {
            mKeyStates[key].Pressed  = pressed;
            mKeyStates[key].Released = !pressed;
        }

        void UpdateMouseButtonState(int button, bool pressed) {
            mMouseStates[button].Pressed  = pressed;
            mMouseStates[button].Released = !pressed;
        }

        void UpdateMousePosition(int x, int y) {
            mMouseX = x;
            mMouseY = y;
        }

    private:
        Input()  = default;
        ~Input() = default;

        struct KeyState {
            bool Pressed  = false;
            bool Released = false;
        };

        std::unordered_map<i32, KeyState> mKeyStates;
        std::unordered_map<i32, KeyState> mMouseStates;
        i32 mMouseX = 0, mMouseY = 0;
    };

}  // namespace Xen