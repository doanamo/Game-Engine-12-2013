#include "Precompiled.hpp"
#include "LuaLogger.hpp"
#include "LuaEngine.hpp"

#include "Common/Services.hpp"
#include "Game/GameState.hpp"
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
#include "Game/Spawn/SpawnSystem.hpp"

bool BindLuaGame(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind component types.
    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<TransformComponent>("TransformComponent")
            .addFunction("SetPosition", &TransformComponent::SetPosition)
            .addFunction("GetPosition", &TransformComponent::GetPositionCopy)
            .addFunction("SetScale", &TransformComponent::SetScale)
            .addFunction("GetScale", &TransformComponent::GetScaleCopy)
            .addFunction("SetRotation", &TransformComponent::SetRotation)
            .addFunction("GetRotation", &TransformComponent::GetRotation)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<InputComponent>("InputComponent")
            .addFunction("SetStateReference", &InputComponent::SetStateReference)
            .addFunction("GetStateReference", &InputComponent::GetStateReference)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<HealthComponent>("HealthComponent")
            .addFunction("SetMaximumHealth", &HealthComponent::SetMaximumHealth)
            .addFunction("GetMaximumHealth", &HealthComponent::GetMaximumHealth)
            .addFunction("SetCurrentHealth", &HealthComponent::SetCurrentHealth)
            .addFunction("GetCurrentHealth", &HealthComponent::GetCurrentHealth)
            .addFunction("IsAlive", &HealthComponent::IsAlive)
            .addFunction("IsDead", &HealthComponent::IsDead)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<CollisionComponent>("CollisionComponent")
            .addFunction("SetBoundingBox", &CollisionComponent::SetBoundingBox)
            .addFunction("GetBoundingBox", &CollisionComponent::GetBoundingBoxCopy)
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

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<ScriptComponent>("ScriptComponent")
            .addFunction("AddScript", &ScriptComponent::AddScript)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<RenderComponent>("RenderComponent")
            .addFunction("SetDiffuseColor", &RenderComponent::SetDiffuseColor)
            .addFunction("GetDiffuseColor", &RenderComponent::GetDiffuseColorCopy)
            .addFunction("SetEmissionColor", &RenderComponent::SetEmissionColor)
            .addFunction("GetEmissionColor", &RenderComponent::GetEmissionColorCopy)
            .addFunction("SetEmissionPower", &RenderComponent::SetEmissionPower)
            .addFunction("GetEmissionPower", &RenderComponent::GetEmissionPower)
        .endClass();

    // Bind system types.
    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<EntityHandle>("EntityHandle")
            .addConstructor<void(*)(void)>()
            .addData("identifier", &EntityHandle::identifier, false)
            .addData("version", &EntityHandle::version, false)
        .endClass()
        .beginClass<EntitySystem>("EntitySystem")
            .addFunction("CreateEntity", &EntitySystem::CreateEntity)
            .addFunction("DestroyEntity", &EntitySystem::DestroyEntity)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<ComponentSystem>("ComponentSystem")
            .addFunction("CreateTransform", &ComponentSystem::Create<TransformComponent>)
            .addFunction("CreateInput", &ComponentSystem::Create<InputComponent>)
            .addFunction("CreateHealth", &ComponentSystem::Create<HealthComponent>)
            .addFunction("CreateCollision", &ComponentSystem::Create<CollisionComponent>)
            .addFunction("CreateScript", &ComponentSystem::Create<ScriptComponent>)
            .addFunction("CreateRender", &ComponentSystem::Create<RenderComponent>)
            .addFunction("LookupTransform", &ComponentSystem::Lookup<TransformComponent>)
            .addFunction("LookupInput", &ComponentSystem::Lookup<InputComponent>)
            .addFunction("LookupHealth", &ComponentSystem::Lookup<HealthComponent>)
            .addFunction("LookupCollision", &ComponentSystem::Lookup<CollisionComponent>)
            .addFunction("LookupScript", &ComponentSystem::Lookup<ScriptComponent>)
            .addFunction("LookupRender", &ComponentSystem::Lookup<RenderComponent>)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<IdentitySystem>("IdentitySystem")
            .addFunction("SetEntityName", &IdentitySystem::SetEntityName)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<InputState>("InputState")
        .endClass()
        .deriveClass<InputSystem, InputState>("InputSystem")
            .addFunction("IsKeyDown", &InputSystem::IsKeyDown)
            .addFunction("IsKeyUp", &InputSystem::IsKeyUp)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<HealthSystem>("HealthSystem")
            .addFunction("Damage", &HealthSystem::Damage)
            .addFunction("Heal", &HealthSystem::Heal)
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
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

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<ScriptSystem>("ScriptSystem")
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<RenderSystem>("RenderSystem")
        .endClass();

    Lua::getGlobalNamespace(lua.GetState())
        .beginClass<SpawnSystem>("SpawnSystem")
            .addFunction("AddSpawn", &SpawnSystem::AddSpawn)
        .endClass();

    Lua::getGlobalNamespace(Main::GetLuaEngine().GetState())
        .beginClass<GameState>("GameState")
            .addConstructor<void(*)(void)>()
            .addFunction("Initialize", &GameState::Initialize)
            .addFunction("Process", &GameState::Process)
            .addFunction("Update", &GameState::Update)
            .addFunction("Draw", &GameState::Draw)
        .endClass();

    // Define constants.
    Lua::LuaRef collisionFlags(lua.GetState());
    collisionFlags = Lua::newTable(lua.GetState());
    collisionFlags["None"] = (uint32_t)CollisionFlags::None;
    collisionFlags["Enabled"] = (uint32_t)CollisionFlags::Enabled;
    collisionFlags["Reversed"] = (uint32_t)CollisionFlags::Reversed;
    collisionFlags["Default"] = (uint32_t)CollisionFlags::Default;

    collisionFlags.push(lua.GetState());
    lua_setglobal(lua.GetState(), "CollisionFlags");

    return true;
}
