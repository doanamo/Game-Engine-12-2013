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

    //
    // Systems
    //

    // Initialize the event system.
    if(!eventSystem.Initialize())
        return false;

    // Initialize the entity system.
    if(!entitySystem.Initialize())
        return false;

    // Initialize the component system.
    if(!componentSystem.Initialize(&entitySystem))
        return false;

    // Initialize the identity system.
    if(!identitySystem.Initialize())
        return false;

    entitySystem.RegisterSubscriber(&identitySystem);

    // Initialize the input system.
    if(!inputSystem.Initialize(&componentSystem))
        return false;

    // Initialize the health system.
    if(!healthSystem.Initialize(&eventSystem, &entitySystem, &identitySystem, &componentSystem))
        return false;

    // Initialize the collision system.
    if(!collisionSystem.Initialize(&eventSystem, &entitySystem, &componentSystem))
        return false;

    // Initialize the script system.
    if(!scriptSystem.Initialize(&eventSystem, &entitySystem, &componentSystem))
        return false;

    // Initialize the render system.
    if(!renderSystem.Initialize(&entitySystem, &componentSystem, 64))
        return false;

    // Initialize the interface system.
    if(!interfaceSystem.Initialize(&eventSystem, &entitySystem, &identitySystem, &componentSystem, &renderSystem))
        return false;

    // Initialize the spawn system.
    if(!spawnSystem.Initialize())
        return false;

    //
    // Success
    //

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

    //
    // Systems
    //

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

    isInitialized = false;
}

//
// State Accessors
//

bool GameState::IsInitialized()
{
    return isInitialized;
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
