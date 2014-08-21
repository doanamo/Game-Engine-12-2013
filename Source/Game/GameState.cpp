#include "Precompiled.hpp"
#include "GameState.hpp"

#include "MainGlobal.hpp"
#include "Scripting/LuaLogger.hpp"
#include "Scripting/LuaMath.hpp"
#include "Scripting/LuaGame.hpp"
#include "Game/Event/EventDefinitions.hpp"

GameState::GameState() :
    m_initialized(false)
{
}

GameState::~GameState()
{
    Cleanup();
}

bool GameState::Initialize()
{
    assert(!m_initialized);

    // Setup scope guard.
    SCOPE_GUARD_IF(!m_initialized, Cleanup());

    // Register services.
    m_services.Set(&m_eventSystem);
    m_services.Set(&m_entitySystem);
    m_services.Set(&m_componentSystem);
    m_services.Set(&m_identitySystem);
    m_services.Set(&m_inputSystem);
    m_services.Set(&m_healthSystem);
    m_services.Set(&m_collisionSystem);
    m_services.Set(&m_scriptSystem);
    m_services.Set(&m_renderSystem);
    m_services.Set(&m_interfaceSystem);
    m_services.Set(&m_spawnSystem);
    m_services.Set(&m_luaEngine);

    // Initialize the event system.
    if(!m_eventSystem.Initialize())
        return false;

    // Initialize the entity system.
    if(!m_entitySystem.Initialize(m_services))
        return false;

    // Initialize the component system.
    if(!m_componentSystem.Initialize(m_services))
        return false;

    // Initialize the identity system.
    if(!m_identitySystem.Initialize(m_services))
        return false;

    // Initialize the input system.
    if(!m_inputSystem.Initialize(m_services))
        return false;

    // Initialize the health system.
    if(!m_healthSystem.Initialize(m_services))
        return false;

    // Initialize the collision system.
    if(!m_collisionSystem.Initialize(m_services))
        return false;

    // Initialize the script system.
    if(!m_scriptSystem.Initialize(m_services))
        return false;

    // Initialize the render system.
    if(!m_renderSystem.Initialize(m_services))
        return false;

    // Initialize the interface system.
    if(!m_interfaceSystem.Initialize(m_services))
        return false;

    // Initialize the spawn system.
    if(!m_spawnSystem.Initialize())
        return false;

    // Initialize the Lua engine.
    if(!m_luaEngine.Initialize())
    {
        Log() << "Failed to initialize Lua engine!";
        return false;
    }

    m_luaEngine.SetPackagePath(Main::GetWorkingDir() + "Data/");
    
    // Setup scripting environment.
    if(!BindLuaLogger(m_luaEngine))
        return false;

    if(!BindLuaMath(m_luaEngine))
        return false;

    if(!BindLuaGame(m_luaEngine, m_services))
        return false;

    // Load the game script.
    if(!m_luaEngine.Load("Data/Game.lua"))
        return false;

    // Call the initialization function.
    m_luaEngine.Call("Game.Initialize");

    // Success!
    return m_initialized = true;
}

void GameState::Cleanup()
{
    // Call the cleanup function.
    if(m_initialized)
    {
        m_luaEngine.Call("Game.Cleanup");
    }

    // Remove all spawn defintions that could have references to Lua.
    m_spawnSystem.RemoveAllSpawns();

    // Entities must be destroyed first, then other systems
    // can be destroyed in a regular reversed order.
    m_entitySystem.DestroyAllEntities();

    // Scripting engine.
    m_luaEngine.Cleanup();

    // Game systems.
    m_spawnSystem.Cleanup();
    m_interfaceSystem.Cleanup();
    m_renderSystem.Cleanup();
    m_scriptSystem.Cleanup();
    m_collisionSystem.Cleanup();
    m_healthSystem.Cleanup();
    m_inputSystem.Cleanup();
    m_identitySystem.Cleanup();
    m_componentSystem.Cleanup();
    m_entitySystem.Cleanup();
    m_eventSystem.Cleanup();

    // Services.
    m_services.Cleanup();

    m_initialized = false;
}

bool GameState::Process(const SDL_Event& event)
{
    assert(m_initialized);

    m_inputSystem.Process(event);

    return false;
}

void GameState::Update(float timeDelta)
{
    assert(m_initialized);

    // Collect scripting garbage.
    m_luaEngine.CollectGarbage(0.02f);

    // Update the spawn system.
    m_spawnSystem.Update(timeDelta);

    // Process entity commands.
    m_entitySystem.ProcessCommands();

    // Update the collision system.
    m_collisionSystem.Update(timeDelta);

    // Update the script system.
    m_scriptSystem.Update(timeDelta);

    // Update the render system.
    m_renderSystem.Update();

    // Update the interface system.
    m_interfaceSystem.Update(timeDelta);
}

void GameState::Draw()
{
    assert(m_initialized);

    // Draw the world.
    m_renderSystem.Draw();

    // Draw the interface.
    m_interfaceSystem.Draw();
}

bool GameState::IsInitialized()
{
    return m_initialized;
}

const Services& GameState::GetServices()
{
    return m_services;
}

EventSystem& GameState::GetEventSystem()
{
    return m_eventSystem;
}

EntitySystem& GameState::GetEntitySystem()
{
    return m_entitySystem;
}

ComponentSystem& GameState::GetComponentSystem()
{
    return m_componentSystem;
}

IdentitySystem& GameState::GetIdentitySystem()
{
    return m_identitySystem;
}

InputSystem& GameState::GetInputSystem()
{
    return m_inputSystem;
}

HealthSystem& GameState::GetHealthSystem()
{
    return m_healthSystem;
}

CollisionSystem& GameState::GetCollisionSystem()
{
    return m_collisionSystem;
}

ScriptSystem& GameState::GetScriptSystem()
{
    return m_scriptSystem;
}

RenderSystem& GameState::GetRenderSystem()
{
    return m_renderSystem;
}

InterfaceSystem& GameState::GetInterfaceSystem()
{
    return m_interfaceSystem;
}

SpawnSystem& GameState::GetSpawnSystem()
{
    return m_spawnSystem;
}

LuaEngine& GameState::GetLuaEngine()
{
    return m_luaEngine;
}
