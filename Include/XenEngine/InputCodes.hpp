// Author: Jake Rieger
// Created: 11/26/2024.
//

#pragma once

#include <Types.hpp>


namespace KeyCode {
    static constexpr u32 Space        = 32;
    static constexpr u32 Apostrophe   = 39;
    static constexpr u32 Comma        = 44;
    static constexpr u32 Minus        = 45;
    static constexpr u32 Period       = 46;
    static constexpr u32 Slash        = 47;
    static constexpr u32 Zero         = 48;
    static constexpr u32 One          = 49;
    static constexpr u32 Two          = 50;
    static constexpr u32 Three        = 51;
    static constexpr u32 Four         = 52;
    static constexpr u32 Five         = 53;
    static constexpr u32 Six          = 54;
    static constexpr u32 Seven        = 55;
    static constexpr u32 Eight        = 56;
    static constexpr u32 Nine         = 57;
    static constexpr u32 Semicolon    = 59;
    static constexpr u32 Equal        = 61;
    static constexpr u32 A            = 65;
    static constexpr u32 B            = 66;
    static constexpr u32 C            = 67;
    static constexpr u32 D            = 68;
    static constexpr u32 E            = 69;
    static constexpr u32 F            = 70;
    static constexpr u32 G            = 71;
    static constexpr u32 H            = 72;
    static constexpr u32 I            = 73;
    static constexpr u32 J            = 74;
    static constexpr u32 K            = 75;
    static constexpr u32 L            = 76;
    static constexpr u32 M            = 77;
    static constexpr u32 N            = 78;
    static constexpr u32 O            = 79;
    static constexpr u32 P            = 80;
    static constexpr u32 Q            = 81;
    static constexpr u32 R            = 82;
    static constexpr u32 S            = 83;
    static constexpr u32 T            = 84;
    static constexpr u32 U            = 85;
    static constexpr u32 V            = 86;
    static constexpr u32 W            = 87;
    static constexpr u32 X            = 88;
    static constexpr u32 Y            = 89;
    static constexpr u32 Z            = 90;
    static constexpr u32 LeftBracket  = 91;
    static constexpr u32 Backslash    = 92;
    static constexpr u32 RightBracket = 93;
    static constexpr u32 GraveAccent  = 96;
    static constexpr u32 World1       = 161;
    static constexpr u32 World2       = 162;
    static constexpr u32 Escape       = 256;
    static constexpr u32 Enter        = 257;
    static constexpr u32 Tab          = 258;
    static constexpr u32 Backspace    = 259;
    static constexpr u32 Insert       = 260;
    static constexpr u32 Delete       = 261;
    static constexpr u32 Right        = 262;
    static constexpr u32 Left         = 263;
    static constexpr u32 Down         = 264;
    static constexpr u32 Up           = 265;
    static constexpr u32 PageUp       = 266;
    static constexpr u32 PageDown     = 267;
    static constexpr u32 Home         = 268;
    static constexpr u32 End          = 269;
    static constexpr u32 CapsLock     = 280;
    static constexpr u32 ScrollLock   = 281;
    static constexpr u32 NumLock      = 282;
    static constexpr u32 PrintScreen  = 283;
    static constexpr u32 Pause        = 284;
    static constexpr u32 F1           = 290;
    static constexpr u32 F2           = 291;
    static constexpr u32 F3           = 292;
    static constexpr u32 F4           = 293;
    static constexpr u32 F5           = 294;
    static constexpr u32 F6           = 295;
    static constexpr u32 F7           = 296;
    static constexpr u32 F8           = 297;
    static constexpr u32 F9           = 298;
    static constexpr u32 F10          = 299;
    static constexpr u32 F11          = 300;
    static constexpr u32 F12          = 301;
    static constexpr u32 F13          = 302;
    static constexpr u32 F14          = 303;
    static constexpr u32 F15          = 304;
    static constexpr u32 F16          = 305;
    static constexpr u32 F17          = 306;
    static constexpr u32 F18          = 307;
    static constexpr u32 F19          = 308;
    static constexpr u32 F20          = 309;
    static constexpr u32 F21          = 310;
    static constexpr u32 F22          = 311;
    static constexpr u32 F23          = 312;
    static constexpr u32 F24          = 313;
    static constexpr u32 F25          = 314;
    static constexpr u32 KP_0         = 320;
    static constexpr u32 KP_1         = 321;
    static constexpr u32 KP_2         = 322;
    static constexpr u32 KP_3         = 323;
    static constexpr u32 KP_4         = 324;
    static constexpr u32 KP_5         = 325;
    static constexpr u32 KP_6         = 326;
    static constexpr u32 KP_7         = 327;
    static constexpr u32 KP_8         = 328;
    static constexpr u32 KP_9         = 329;
    static constexpr u32 KP_DECIMAL   = 330;
    static constexpr u32 KP_DIVIDE    = 331;
    static constexpr u32 KP_MULTIPLY  = 332;
    static constexpr u32 KP_SUBTRACT  = 333;
    static constexpr u32 KP_ADD       = 334;
    static constexpr u32 KP_ENTER     = 335;
    static constexpr u32 KP_EQUAL     = 336;
    static constexpr u32 LeftShift    = 340;
    static constexpr u32 LeftControl  = 341;
    static constexpr u32 LeftAlt      = 342;
    static constexpr u32 LeftSuper    = 343;
    static constexpr u32 RightShift   = 344;
    static constexpr u32 RightControl = 345;
    static constexpr u32 RightAlt     = 346;
    static constexpr u32 RightSuper   = 347;
    static constexpr u32 RightMenu    = 348;
}  // namespace KeyCode

namespace MouseButton {
    static constexpr u32 Left   = 0;
    static constexpr u32 Right  = 1;
    static constexpr u32 Middle = 2;
    static constexpr u32 Macro1 = 3;
    static constexpr u32 Macro2 = 4;
    static constexpr u32 Macro3 = 5;
    static constexpr u32 Macro4 = 6;
    static constexpr u32 Macro5 = 7;
}