#include "Precompiled.hpp"
#include "LuaSystem.hpp"
#include "LuaEngine.hpp"

bool BindLuaSystem(LuaEngine& state)
{
    if(!state.IsValid())
        return false;

    // Bind definitions.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<SDL_Event>("SystemEvent")
        .endClass();

    return true;
}
