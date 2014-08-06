#pragma once

#include "Precompiled.hpp"
#include "Common/Services.hpp"

// Forward declarations.
class EventSystem;
class EntitySystem;
class ComponentSystem;
class IdentitySystem;
class InputSystem;
class HealthSystem;
class CollisionSystem;
class ScriptSystem;
class RenderSystem;
class InterfaceSystem;
class SpawnSystem;
class ProgressSystem;
class LuaEngine;

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

    const Services& GetServices();

    EventSystem&     GetEventSystem();
    EntitySystem&    GetEntitySystem();
    ComponentSystem& GetComponentSystem();
    IdentitySystem&  GetIdentitySystem();
    InputSystem&     GetInputSystem();
    HealthSystem&    GetHealthSystem();
    CollisionSystem& GetCollisionSystem();
    ScriptSystem&    GetScriptSystem();
    RenderSystem&    GetRenderSystem();
    InterfaceSystem& GetInterfaceSystem();
    SpawnSystem&     GetSpawnSystem();

    LuaEngine& GetLuaEngine();
};
