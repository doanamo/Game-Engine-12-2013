#include "Precompiled.hpp"
#include "LuaSystem.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"

bool BindLuaSystem(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginNamespace("System")
            .addFunction("Quit", &Main::Quit)
            .beginClass<SDL_Event>("Event")
            .endClass()
        .endNamespace();

    return true;
}
