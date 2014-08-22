#include "Precompiled.hpp"
#include "LuaSystem.hpp"
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

    // Define constants.
    Lua::LuaRef eventTypes(lua.GetState());
    eventTypes = Lua::newTable(lua.GetState());
    eventTypes["KeyDown"] = (uint32_t)SDL_KEYDOWN;
    eventTypes["KeyUp"] = (uint32_t)SDL_KEYUP;
    eventTypes["Quit"] = (uint32_t)SDL_QUIT;

    eventTypes.push(lua.GetState());
    lua_setglobal(lua.GetState(), "Event");

    return true;
}
