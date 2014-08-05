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
#include "Game/Health/HealthComponent.hpp"
#include "Game/Health/HealthSystem.hpp"
#include "Game/Collision/CollisionComponent.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Script/ScriptComponent.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Render/RenderComponent.hpp"
#include "Game/Render/RenderSystem.hpp"

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

    HealthSystem* healthSystem = services.Get<HealthSystem>();
    if(healthSystem == nullptr) return false;

    CollisionSystem* collisionSystem = services.Get<CollisionSystem>();
    if(collisionSystem == nullptr) return false;

    ScriptSystem* scriptSystem = services.Get<ScriptSystem>();
    if(scriptSystem == nullptr) return false;

    RenderSystem* renderSystem = services.Get<RenderSystem>();
    if(renderSystem == nullptr) return false;

    // Bind component types.
    Lua::getGlobalNamespace(state.GetState())
        .beginClass<TransformComponent>("TransformComponent")
            .addFunction("SetPosition", &TransformComponent::SetPosition)
            .addFunction("GetPosition", &TransformComponent::GetPosition)
            .addFunction("SetRotation", &TransformComponent::SetRotation)
            .addFunction("GetRotation", &TransformComponent::GetRotation)
            .addFunction("SetScale", &TransformComponent::SetScale)
            .addFunction("GetScale", &TransformComponent::GetScale)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<InputComponent>("InputComponent")
            .addFunction("SetStateReference", &InputComponent::SetStateReference)
            .addFunction("GetStateReference", &InputComponent::GetStateReference)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<HealthComponent>("HealthComponent")
            .addFunction("SetMaximumHealth", &HealthComponent::SetMaximumHealth)
            .addFunction("GetMaximumHealth", &HealthComponent::GetMaximumHealth)
            .addFunction("SetCurrentHealth", &HealthComponent::SetCurrentHealth)
            .addFunction("GetCurrentHealth", &HealthComponent::GetCurrentHealth)
            .addFunction("IsAlive", &HealthComponent::IsAlive)
            .addFunction("IsDead", &HealthComponent::IsDead)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<CollisionComponent>("CollisionComponent")
            .addFunction("SetBoundingBox", &CollisionComponent::SetBoundingBox)
            .addFunction("GetBoundingBox", &CollisionComponent::GetBoundingBox)
            .addFunction("SetType", &CollisionComponent::SetType)
            .addFunction("GetType", &CollisionComponent::GetType)
            .addFunction("SetMask", &CollisionComponent::SetMask)
            .addFunction("GetMask", &CollisionComponent::GetMask)
            .addFunction("SetFlags", &CollisionComponent::SetFlags)
            .addFunction("GetFlags", &CollisionComponent::GetFlags)
            .addFunction("Enable", &CollisionComponent::Enable)
            .addFunction("Disable", &CollisionComponent::Disable)
            .addFunction("IsEnabled", &CollisionComponent::IsEnabled)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<ScriptLuaComponent>("ScriptComponent")
            .addFunction("AddScript", &ScriptLuaComponent::AddScript)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<RenderComponent>("RenderComponent")
            .addFunction("SetDiffuseColor", &RenderComponent::SetDiffuseColor)
            .addFunction("GetDiffuseColor", &RenderComponent::GetDiffuseColor)
            .addFunction("SetEmissionColor", &RenderComponent::SetEmissionColor)
            .addFunction("GetEmissionColor", &RenderComponent::GetEmissionColor)
            .addFunction("SetEmissionPower", &RenderComponent::SetEmissionPower)
            .addFunction("GetEmissionPower", &RenderComponent::GetEmissionPower)
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
            .addFunction("CreateHealth", &ComponentSystem::Create<HealthComponent>)
            .addFunction("CreateCollision", &ComponentSystem::Create<CollisionComponent>)
            .addFunction("CreateScript", &ComponentSystem::Create<ScriptLuaComponent>)
            .addFunction("CreateRender", &ComponentSystem::Create<RenderComponent>)
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

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<HealthSystem>("HealthSystem")
            .addFunction("Damage", &HealthSystem::Damage)
            .addFunction("Heal", &HealthSystem::Heal)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<CollisionObject>("CollisionObject")
            .addData("entity", &CollisionObject::entity, false)
            .addData("transform", &CollisionObject::transform, false)
            .addData("collision", &CollisionObject::collision, false)
            .addData("worldAABB", &CollisionObject::worldAABB, false)
            .addData("enabled", &CollisionObject::enabled, false)
        .endClass()
        .beginClass<CollisionSystem>("CollisionSystem")
            .addFunction("DisableCollisionResponse", &CollisionSystem::DisableCollisionResponse)
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<ScriptSystem>("ScriptSystem")
        .endClass();

    Lua::getGlobalNamespace(state.GetState())
        .beginClass<RenderSystem>("RenderSystem")
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

    Lua::push(state.GetState(), healthSystem);
    lua_setglobal(state.GetState(), "HealthSystem");

    Lua::push(state.GetState(), collisionSystem);
    lua_setglobal(state.GetState(), "CollisionSystem");

    Lua::push(state.GetState(), scriptSystem);
    lua_setglobal(state.GetState(), "ScriptSystem");

    Lua::push(state.GetState(), renderSystem);
    lua_setglobal(state.GetState(), "RenderSystem");

    return true;
}
