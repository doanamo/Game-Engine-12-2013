#include "Precompiled.hpp"
#include "LuaLogger.hpp"
#include "LuaState.hpp"

#include "Common/Services.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Input/InputComponent.hpp"
#include "Game/Input/InputSystem.hpp"

bool BindLuaGame(LuaState& state, const Services& services)
{
    if(!state.IsValid())
        return false;

    // Get required systems.
    EntitySystem* entitySystem = services.Get<EntitySystem>();
    if(entitySystem == nullptr) return false;

    ComponentSystem* componentSystem = services.Get<ComponentSystem>();
    if(componentSystem == nullptr) return false;

    IdentitySystem* identitySystem = services.Get<IdentitySystem>();
    if(identitySystem == nullptr) return false;

    InputSystem* inputSystem = services.Get<InputSystem>();
    if(inputSystem == nullptr) return false;

    // Bind component types.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<TransformComponent>("TransformComponent")
            .addFunction("SetPosition", &TransformComponent::SetPosition)
            .addFunction("SetRotation", &TransformComponent::SetRotation)
            .addFunction("SetScale", &TransformComponent::SetScale)
            .addFunction("GetPosition", &TransformComponent::GetPosition)
            .addFunction("GetRotation", &TransformComponent::GetRotation)
            .addFunction("GetScale", &TransformComponent::GetScale)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<InputComponent>("InputComponent")
            .addFunction("SetStateReference", &InputComponent::SetStateReference)
            .addFunction("GetStateReference", &InputComponent::GetStateReference)
        .endClass();

    // Bind system types.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<EntityHandle>("EntityHandle")
            .addConstructor<void(*)(void)>()
            .addData("identifier", &EntityHandle::identifier, false)
            .addData("version", &EntityHandle::version, false)
        .endClass()
        .beginClass<EntitySystem>("EntitySystem")
            .addFunction("CreateEntity", &EntitySystem::CreateEntity)
            .addFunction("DestroyEntity", &EntitySystem::DestroyEntity)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<ComponentSystem>("ComponentSystem")
            .addFunction("CreateTransform", &ComponentSystem::Create<TransformComponent>)
            .addFunction("CreateInput", &ComponentSystem::Create<InputComponent>)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<IdentitySystem>("IdentitySystem")
            .addFunction("SetEntityName", &IdentitySystem::SetEntityName)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<InputState>("InputState")
        .endClass()
        .deriveClass<InputSystem, InputState>("InputSystem")
        .endClass();

    // Pass object references.
    Lua::push(state.GetState(), entitySystem);
    lua_setglobal(state.GetState(), "EntitySystem");

    Lua::push(state.GetState(), componentSystem);
    lua_setglobal(state.GetState(), "ComponentSystem");

    Lua::push(state.GetState(), identitySystem);
    lua_setglobal(state.GetState(), "IdentitySystem");

    Lua::push(state.GetState(), inputSystem);
    lua_setglobal(state.GetState(), "InputSystem");

    return true;
}
