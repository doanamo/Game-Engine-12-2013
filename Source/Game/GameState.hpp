#pragma once

#include "Precompiled.hpp"

#include "Game/Component/ComponentPool.hpp"

// Forward declarations.
class TransformComponent;
class InputComponent;
class HealthComponent;
class CollisionComponent;
class ScriptComponent;
class RenderComponent;

class InputState;

class EntitySystem;
class IdentitySystem;
class HealthSystem;
class CollisionSystem;
class ScriptSystem;
class RenderSystem;
class InterfaceSystem;
class SpawnSystem;
class ProgressSystem;

//
// Game State
//

namespace GameState
{
    // State methods.
    bool Initialize();
    void Cleanup();

    // State accessors.
    bool IsInitialized();

    ComponentPool<TransformComponent>& GetTransformComponents();
    ComponentPool<InputComponent>&     GetInputComponents();
    ComponentPool<HealthComponent>&    GetHealthComponents();
    ComponentPool<CollisionComponent>& GetCollisionComponents();
    ComponentPool<ScriptComponent>&    GetScriptComponents();
    ComponentPool<RenderComponent>&    GetRenderComponents();

    InputState& GetInputState();

    EntitySystem&    GetEntitySystem();
    IdentitySystem&  GetIdentitySystem();
    HealthSystem&    GetHealthSystem();
    CollisionSystem& GetCollisionSystem();
    ScriptSystem&    GetScriptSystem();
    RenderSystem&    GetRenderSystem();
    InterfaceSystem& GetInterfaceSystem();

    SpawnSystem&     GetSpawnSystem();
    ProgressSystem&  GetProgressSystem();
};
