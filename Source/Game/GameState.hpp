#pragma once

#include "Precompiled.hpp"

#include "Common/Services.hpp"
#include "Scripting/LuaEngine.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputSystem.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Health/HealthSystem.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Render/RenderSystem.hpp"
#include "Game/Interface/InterfaceSystem.hpp"
#include "Game/Spawn/SpawnSystem.hpp"

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

class GameState
{
public:
    GameState();
    ~GameState();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float timeDelta);
    void Draw();

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

private:
    bool m_initialized;

    // Service collection.
    Services m_services;

    // Game systems.
    EventSystem     m_eventSystem;
    EntitySystem    m_entitySystem;
    ComponentSystem m_componentSystem; 
    IdentitySystem  m_identitySystem;
    InputSystem     m_inputSystem;
    HealthSystem    m_healthSystem;
    CollisionSystem m_collisionSystem;
    ScriptSystem    m_scriptSystem;
    RenderSystem    m_renderSystem;
    InterfaceSystem m_interfaceSystem;
    SpawnSystem     m_spawnSystem;

    // Scripting engine.
    LuaEngine m_luaEngine;
};
