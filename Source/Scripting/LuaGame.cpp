#include "Precompiled.hpp"
#include "LuaLogger.hpp"
#include "LuaState.hpp"

#include "Common/Services.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"

bool BindLuaGame(LuaState& state, const Services& services)
{
    if(!state.IsValid())
        return false;

    // Get required systems.
    EntitySystem* entitySystem = services.Get<EntitySystem>();
    if(entitySystem == nullptr) return false;

    IdentitySystem* identitySystem = services.Get<IdentitySystem>();
    if(identitySystem == nullptr) return false;

    // Bind definitions.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<EntityHandle>("EntityHandle")
            .addConstructor<void(*)(void)>()
            .addData("identifier", &EntityHandle::identifier, false)
            .addData("version", &EntityHandle::version, false)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<EntitySystem>("EntitySystem")
            .addFunction("CreateEntity", &EntitySystem::CreateEntity)
            .addFunction("DestroyEntity", &EntitySystem::DestroyEntity)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<IdentitySystem>("IdentitySystem")
            .addFunction("SetEntityName", &IdentitySystem::SetEntityName)
        .endClass();

    // Pass object references.
    Lua::push(state.GetState(), entitySystem);
    lua_setglobal(state.GetState(), "EntitySystem");

    Lua::push(state.GetState(), identitySystem);
    lua_setglobal(state.GetState(), "IdentitySystem");

    return true;
}
