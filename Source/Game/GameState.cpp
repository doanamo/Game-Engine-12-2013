#include "Precompiled.hpp"
#include "GameState.hpp"

#include "Game/Event/EventDefinitions.hpp"
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

//
// State Data
//

namespace
{
    bool isInitialized = false;

    // Services.
    Services services;

    // Game systems.
    EventSystem     eventSystem;
    EntitySystem    entitySystem;
    ComponentSystem componentSystem; 
    IdentitySystem  identitySystem;
    InputSystem     inputSystem;
    HealthSystem    healthSystem;
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;
    InterfaceSystem interfaceSystem;
    SpawnSystem     spawnSystem;
}

//
// State Methods
//

bool GameState::Initialize()
{
    assert(!isInitialized);

    // Register services.
    services.Set(&eventSystem);
    services.Set(&entitySystem);
    services.Set(&componentSystem);
    services.Set(&identitySystem);
    services.Set(&inputSystem);
    services.Set(&healthSystem);
    services.Set(&collisionSystem);
    services.Set(&scriptSystem);
    services.Set(&renderSystem);
    services.Set(&interfaceSystem);
    services.Set(&spawnSystem);

    // Initialize the event system.
    if(!eventSystem.Initialize())
        return false;

    // Initialize the entity system.
    if(!entitySystem.Initialize())
        return false;

    // Initialize the component system.
    if(!componentSystem.Initialize(services))
        return false;

    // Initialize the identity system.
    if(!identitySystem.Initialize())
        return false;

    entitySystem.RegisterSubscriber(&identitySystem);

    // Initialize the input system.
    if(!inputSystem.Initialize(services))
        return false;

    // Initialize the health system.
    if(!healthSystem.Initialize(services))
        return false;

    // Initialize the collision system.
    if(!collisionSystem.Initialize(services))
        return false;

    // Initialize the script system.
    if(!scriptSystem.Initialize(services))
        return false;

    // Initialize the render system.
    if(!renderSystem.Initialize(services))
        return false;

    // Initialize the interface system.
    if(!interfaceSystem.Initialize(services))
        return false;

    // Initialize the spawn system.
    if(!spawnSystem.Initialize())
        return false;

    // Success!
    return isInitialized = true;
}

void GameState::Cleanup()
{
    if(!isInitialized)
        return;

    Log() << "Cleaning up the game state...";

    // Entities must be destroyed first, then other systems
    // can be destroyed in a regular reversed order.
    entitySystem.DestroyAllEntities();

    // Game systems.
    spawnSystem.Cleanup();
    interfaceSystem.Cleanup();
    renderSystem.Cleanup();
    scriptSystem.Cleanup();
    collisionSystem.Cleanup();
    healthSystem.Cleanup();
    inputSystem.Cleanup();
    identitySystem.Cleanup();
    componentSystem.Cleanup();
    entitySystem.Cleanup();
    eventSystem.Cleanup();

    // Services.
    services.Cleanup();

    isInitialized = false;
}

//
// State Accessors
//

bool GameState::IsInitialized()
{
    return isInitialized;
}

Services& GameState::GetServices()
{
    return services;
}

EventSystem& GameState::GetEventSystem()
{
    return eventSystem;
}

EntitySystem& GameState::GetEntitySystem()
{
    return entitySystem;
}

ComponentSystem& GameState::GetComponentSystem()
{
    return componentSystem;
}

IdentitySystem& GameState::GetIdentitySystem()
{
    return identitySystem;
}

InputSystem& GameState::GetInputSystem()
{
    return inputSystem;
}

HealthSystem& GameState::GetHealthSystem()
{
    return healthSystem;
}

CollisionSystem& GameState::GetCollisionSystem()
{
    return collisionSystem;
}

ScriptSystem& GameState::GetScriptSystem()
{
    return scriptSystem;
}

RenderSystem& GameState::GetRenderSystem()
{
    return renderSystem;
}

InterfaceSystem& GameState::GetInterfaceSystem()
{
    return interfaceSystem;
}

SpawnSystem& GameState::GetSpawnSystem()
{
    return spawnSystem;
}
