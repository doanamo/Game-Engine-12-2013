#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class InputState;

class EventSystem;
class EntitySystem;
class ComponentSystem;
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

    InputState& GetInputState();

    EventSystem&     GetEventSystem();
    EntitySystem&    GetEntitySystem();
    ComponentSystem& GetComponentSystem();
    IdentitySystem&  GetIdentitySystem();
    HealthSystem&    GetHealthSystem();
    CollisionSystem& GetCollisionSystem();
    ScriptSystem&    GetScriptSystem();
    RenderSystem&    GetRenderSystem();
    InterfaceSystem& GetInterfaceSystem();
    SpawnSystem&     GetSpawnSystem();
};
