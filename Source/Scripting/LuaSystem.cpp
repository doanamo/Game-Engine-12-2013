#include "Precompiled.hpp"
#include "LuaSystem.hpp"
#include "LuaEngine.hpp"

bool BindLuaSystem(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<SDL_Event>("SystemEvent")
        .endClass();

    return true;
}
