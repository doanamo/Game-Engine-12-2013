#include "Precompiled.hpp"
#include "LuaBindings.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"

namespace
{
    // Proxy functions.
    uint32_t GetKeyboardEventScancode(const SDL_KeyboardEvent& event)
    {
        return event.keysym.scancode;
    }
}

bool BindLuaSystem(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("System")
            .addFunction("Quit", &Main::Quit)
            .beginClass<SDL_KeyboardEvent>("KeyboardEvent")
                .addPropertyProxy("scancode", &GetKeyboardEventScancode)
            .endClass()
            .beginClass<SDL_QuitEvent>("QuitEvent")
            .endClass()
            .beginClass<SDL_Event>("Event")
                .addData("type", &SDL_Event::type)
                .addData("key", &SDL_Event::key)
                .addData("quit", &SDL_Event::quit)
            .endClass()
        .endNamespace();

    // Define event types.
    Lua::LuaRef eventTypes(lua.GetState());
    eventTypes = Lua::newTable(lua.GetState());

    eventTypes["KeyDown"] = (uint32_t)SDL_KEYDOWN;
    eventTypes["KeyUp"] = (uint32_t)SDL_KEYUP;
    eventTypes["Quit"] = (uint32_t)SDL_QUIT;

    eventTypes.push(lua.GetState());
    lua_setglobal(lua.GetState(), "Event");

    // Define keyboard scancodes.
    Lua::LuaRef keyCodes(lua.GetState());
    keyCodes = Lua::newTable(lua.GetState());

    keyCodes["A"]            = (uint32_t)SDL_SCANCODE_A;
    keyCodes["B"]            = (uint32_t)SDL_SCANCODE_B;
    keyCodes["C"]            = (uint32_t)SDL_SCANCODE_C;
    keyCodes["D"]            = (uint32_t)SDL_SCANCODE_D;
    keyCodes["E"]            = (uint32_t)SDL_SCANCODE_E;
    keyCodes["F"]            = (uint32_t)SDL_SCANCODE_F;
    keyCodes["G"]            = (uint32_t)SDL_SCANCODE_G;
    keyCodes["H"]            = (uint32_t)SDL_SCANCODE_H;
    keyCodes["I"]            = (uint32_t)SDL_SCANCODE_I;
    keyCodes["J"]            = (uint32_t)SDL_SCANCODE_J;
    keyCodes["K"]            = (uint32_t)SDL_SCANCODE_K;
    keyCodes["L"]            = (uint32_t)SDL_SCANCODE_L;
    keyCodes["M"]            = (uint32_t)SDL_SCANCODE_M;
    keyCodes["N"]            = (uint32_t)SDL_SCANCODE_N;
    keyCodes["O"]            = (uint32_t)SDL_SCANCODE_O;
    keyCodes["P"]            = (uint32_t)SDL_SCANCODE_P;
    keyCodes["Q"]            = (uint32_t)SDL_SCANCODE_Q;
    keyCodes["R"]            = (uint32_t)SDL_SCANCODE_R;
    keyCodes["S"]            = (uint32_t)SDL_SCANCODE_S;
    keyCodes["T"]            = (uint32_t)SDL_SCANCODE_T;
    keyCodes["U"]            = (uint32_t)SDL_SCANCODE_U;
    keyCodes["V"]            = (uint32_t)SDL_SCANCODE_V;
    keyCodes["W"]            = (uint32_t)SDL_SCANCODE_W;
    keyCodes["X"]            = (uint32_t)SDL_SCANCODE_X;
    keyCodes["Y"]            = (uint32_t)SDL_SCANCODE_Y;
    keyCodes["Z"]            = (uint32_t)SDL_SCANCODE_Z;

    keyCodes["1"]            = (uint32_t)SDL_SCANCODE_1;
    keyCodes["2"]            = (uint32_t)SDL_SCANCODE_2;
    keyCodes["3"]            = (uint32_t)SDL_SCANCODE_3;
    keyCodes["4"]            = (uint32_t)SDL_SCANCODE_4;
    keyCodes["5"]            = (uint32_t)SDL_SCANCODE_5;
    keyCodes["6"]            = (uint32_t)SDL_SCANCODE_6;
    keyCodes["7"]            = (uint32_t)SDL_SCANCODE_7;
    keyCodes["8"]            = (uint32_t)SDL_SCANCODE_8;
    keyCodes["9"]            = (uint32_t)SDL_SCANCODE_9;
    keyCodes["0"]            = (uint32_t)SDL_SCANCODE_0;

    keyCodes["Return"]       = (uint32_t)SDL_SCANCODE_RETURN;
    keyCodes["Escape"]       = (uint32_t)SDL_SCANCODE_ESCAPE;
    keyCodes["Backspace"]    = (uint32_t)SDL_SCANCODE_BACKSPACE;
    keyCodes["Tab"]          = (uint32_t)SDL_SCANCODE_TAB;
    keyCodes["Space"]        = (uint32_t)SDL_SCANCODE_SPACE;

    keyCodes["Minus"]        = (uint32_t)SDL_SCANCODE_MINUS;
    keyCodes["Equals"]       = (uint32_t)SDL_SCANCODE_EQUALS;
    keyCodes["LeftBracket"]  = (uint32_t)SDL_SCANCODE_LEFTBRACKET;
    keyCodes["RightBracker"] = (uint32_t)SDL_SCANCODE_RIGHTBRACKET;
    keyCodes["BackSlash"]    = (uint32_t)SDL_SCANCODE_BACKSLASH;
    keyCodes["Semicolon"]    = (uint32_t)SDL_SCANCODE_SEMICOLON;
    keyCodes["Apostrophe"]   = (uint32_t)SDL_SCANCODE_APOSTROPHE;
    keyCodes["Grave"]        = (uint32_t)SDL_SCANCODE_GRAVE;
    keyCodes["Comma"]        = (uint32_t)SDL_SCANCODE_COMMA;
    keyCodes["Period"]       = (uint32_t)SDL_SCANCODE_PERIOD;
    keyCodes["Slash"]        = (uint32_t)SDL_SCANCODE_SLASH;

    keyCodes["CapsLock"]     = (uint32_t)SDL_SCANCODE_CAPSLOCK;

    keyCodes["F1"]           = (uint32_t)SDL_SCANCODE_F1;
    keyCodes["F2"]           = (uint32_t)SDL_SCANCODE_F2;
    keyCodes["F3"]           = (uint32_t)SDL_SCANCODE_F3;
    keyCodes["F4"]           = (uint32_t)SDL_SCANCODE_F4;
    keyCodes["F5"]           = (uint32_t)SDL_SCANCODE_F5;
    keyCodes["F6"]           = (uint32_t)SDL_SCANCODE_F6;
    keyCodes["F7"]           = (uint32_t)SDL_SCANCODE_F7;
    keyCodes["F8"]           = (uint32_t)SDL_SCANCODE_F8;
    keyCodes["F9"]           = (uint32_t)SDL_SCANCODE_F9;
    keyCodes["F10"]          = (uint32_t)SDL_SCANCODE_F10;
    keyCodes["F11"]          = (uint32_t)SDL_SCANCODE_F11;
    keyCodes["F12"]          = (uint32_t)SDL_SCANCODE_F12;

    keyCodes["PrintScreen"]  = (uint32_t)SDL_SCANCODE_PRINTSCREEN;
    keyCodes["ScrollLock"]   = (uint32_t)SDL_SCANCODE_SCROLLLOCK;
    keyCodes["Pause"]        = (uint32_t)SDL_SCANCODE_PAUSE;
    keyCodes["Insert"]       = (uint32_t)SDL_SCANCODE_INSERT;

    keyCodes["Home"]         = (uint32_t)SDL_SCANCODE_HOME;
    keyCodes["PageUp"]       = (uint32_t)SDL_SCANCODE_PAGEUP;
    keyCodes["Delete"]       = (uint32_t)SDL_SCANCODE_DELETE;
    keyCodes["End"]          = (uint32_t)SDL_SCANCODE_END;
    keyCodes["PageDown"]     = (uint32_t)SDL_SCANCODE_PAGEDOWN;
    keyCodes["Right"]        = (uint32_t)SDL_SCANCODE_RIGHT;
    keyCodes["Left"]         = (uint32_t)SDL_SCANCODE_LEFT;
    keyCodes["Down"]         = (uint32_t)SDL_SCANCODE_DOWN;
    keyCodes["Up"]           = (uint32_t)SDL_SCANCODE_UP;

    keyCodes["NumLock"]      = (uint32_t)SDL_SCANCODE_NUMLOCKCLEAR;

    keyCodes["NumDivide"]    = (uint32_t)SDL_SCANCODE_KP_DIVIDE;
    keyCodes["NumMultiply"]  = (uint32_t)SDL_SCANCODE_KP_MULTIPLY;
    keyCodes["NumMinus"]     = (uint32_t)SDL_SCANCODE_KP_MINUS;
    keyCodes["NumPlus"]      = (uint32_t)SDL_SCANCODE_KP_PLUS;
    keyCodes["NumEnter"]     = (uint32_t)SDL_SCANCODE_KP_ENTER;
    keyCodes["Num1"]         = (uint32_t)SDL_SCANCODE_KP_1;
    keyCodes["Num2"]         = (uint32_t)SDL_SCANCODE_KP_2;
    keyCodes["Num3"]         = (uint32_t)SDL_SCANCODE_KP_3;
    keyCodes["Num4"]         = (uint32_t)SDL_SCANCODE_KP_4;
    keyCodes["Num5"]         = (uint32_t)SDL_SCANCODE_KP_5;
    keyCodes["Num6"]         = (uint32_t)SDL_SCANCODE_KP_6;
    keyCodes["Num7"]         = (uint32_t)SDL_SCANCODE_KP_7;
    keyCodes["Num8"]         = (uint32_t)SDL_SCANCODE_KP_8;
    keyCodes["Num9"]         = (uint32_t)SDL_SCANCODE_KP_9;
    keyCodes["Num0"]         = (uint32_t)SDL_SCANCODE_KP_0;
    keyCodes["NumPeriod"]    = (uint32_t)SDL_SCANCODE_KP_PERIOD;

    keyCodes["LeftControl"]  = (uint32_t)SDL_SCANCODE_LCTRL;
    keyCodes["LeftShift"]    = (uint32_t)SDL_SCANCODE_LSHIFT;
    keyCodes["LeftAlt"]      = (uint32_t)SDL_SCANCODE_LALT;
    keyCodes["LeftSystem"]   = (uint32_t)SDL_SCANCODE_LGUI;
    keyCodes["RightControl"] = (uint32_t)SDL_SCANCODE_RCTRL;
    keyCodes["RightShift"]   = (uint32_t)SDL_SCANCODE_RSHIFT;
    keyCodes["RightAlt"]     = (uint32_t)SDL_SCANCODE_RALT;
    keyCodes["RightSystem"]  = (uint32_t)SDL_SCANCODE_RGUI;

    keyCodes.push(lua.GetState());
    lua_setglobal(lua.GetState(), "Key");

    return true;
}
