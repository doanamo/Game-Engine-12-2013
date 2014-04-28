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

    ComponentPool<TransformComponent>& TransformComponents();
    ComponentPool<InputComponent>&     InputComponents();
    ComponentPool<HealthComponent>&    HealthComponents();
    ComponentPool<CollisionComponent>& CollisionComponents();
    ComponentPool<ScriptComponent>&    ScriptComponents();
    ComponentPool<RenderComponent>&    RenderComponents();

    InputState& InputState();

    EntitySystem&    EntitySystem();
    IdentitySystem&  IdentitySystem();
    HealthSystem&    HealthSystem();
    CollisionSystem& CollisionSystem();
    ScriptSystem&    ScriptSystem();
    RenderSystem&    RenderSystem();
    InterfaceSystem& InterfaceSystem();

    SpawnSystem&     SpawnSystem();
    ProgressSystem&  ProgressSystem();
};
