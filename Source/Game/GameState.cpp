#include "Precompiled.hpp"
#include "GameState.hpp"

#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputState.hpp"
#include "Game/Input/InputComponent.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Collision/CollisionComponent.hpp"
#include "Game/Health/HealthSystem.hpp"
#include "Game/Health/HealthComponent.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Script/ScriptComponent.hpp"
#include "Game/Render/RenderSystem.hpp"
#include "Game/Render/RenderComponent.hpp"
#include "Game/Interface/InterfaceSystem.hpp"
#include "Game/Spawn/SpawnSystem.hpp"
#include "Game/Progress/ProgressSystem.hpp"

//
// State Data
//

namespace
{
    bool isInitialized = false;

    // Component pools.
    ComponentPool<TransformComponent> transformComponents;
    ComponentPool<InputComponent>     inputComponents;
    ComponentPool<HealthComponent>    healthComponents;
    ComponentPool<CollisionComponent> collisionComponents;
    ComponentPool<ScriptComponent>    scriptComponents;
    ComponentPool<RenderComponent>    renderComponents;

    // Input state.
    InputState inputState;

    // Entity systems.
    EntitySystem    entitySystem;
    IdentitySystem  identitySystem;
    HealthSystem    healthSystem;
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;
    InterfaceSystem interfaceSystem;

    // Game systems.
    SpawnSystem     spawnSystem;
    ProgressSystem  progressSystem;
}

//
// State Methods
//

bool GameState::Initialize()
{
    assert(!isInitialized);

    //
    // Input State
    //

    // Initialize the input state.
    if(!inputState.Initialize())
        return false;

    //
    // Entity Systems
    //

    // Initialize the identity system.
    if(!identitySystem.Initialize())
        return false;

    entitySystem.RegisterSubscriber(&identitySystem);

    // Initialize the health system.
    if(!healthSystem.Initialize())
        return false;

    // Initialize the collision system.
    if(!collisionSystem.Initialize())
        return false;

    // Initialize the script system.
    if(!scriptSystem.Initialize())
        return false;

    // Initialize the render system.
    if(!renderSystem.Initialize(64))
        return false;

    // Initialize the interface system.
    if(!interfaceSystem.Initialize())
        return false;

    healthSystem.SubscribeReceiver(interfaceSystem.GetHealthChangeReceiver());

    //
    // Component Pools
    //

    // Subscribe component pools to the entity system.
    entitySystem.RegisterSubscriber(&transformComponents);
    entitySystem.RegisterSubscriber(&inputComponents);
    entitySystem.RegisterSubscriber(&healthComponents);
    entitySystem.RegisterSubscriber(&collisionComponents);
    entitySystem.RegisterSubscriber(&scriptComponents);
    entitySystem.RegisterSubscriber(&renderComponents);

    //
    // Game Systems
    //

    // Initialize the spawn system.
    if(!spawnSystem.Initialize())
        return false;

    // Initialize the progress system.
    if(!progressSystem.Initialize())
        return false;

    //
    // Success
    //

    isInitialized = true;

    return true;
}

void GameState::Cleanup()
{
    if(!isInitialized)
        return;

    Log() << "Cleaning up the game state...";

    //
    // Input State
    //

    inputState.Cleanup();

    //
    // Game Systems
    //
    
    progressSystem.Cleanup();
    spawnSystem.Cleanup();

    //
    // Entity Systems
    //
    
    interfaceSystem.Cleanup();
    renderSystem.Cleanup();
    scriptSystem.Cleanup();
    collisionSystem.Cleanup();
    healthSystem.Cleanup();
    identitySystem.Cleanup();
    entitySystem.Cleanup();

    //
    // Component Pools
    //

    transformComponents.Cleanup();
    inputComponents.Cleanup();
    healthComponents.Cleanup();
    collisionComponents.Cleanup();
    scriptComponents.Cleanup();
    renderComponents.Cleanup();

    //
    // Game
    //

    isInitialized = false;
}

//
// State Accessors
//

bool GameState::IsInitialized()
{
    return isInitialized;
}

ComponentPool<TransformComponent>& GameState::TransformComponents()
{
    return transformComponents;
}

ComponentPool<InputComponent>& GameState::InputComponents()
{
    return inputComponents;
}

ComponentPool<CollisionComponent>& GameState::CollisionComponents()
{
    return collisionComponents;
}

ComponentPool<HealthComponent>& GameState::HealthComponents()
{
    return healthComponents;
}

ComponentPool<ScriptComponent>& GameState::ScriptComponents()
{
    return scriptComponents;
}

ComponentPool<RenderComponent>& GameState::RenderComponents()
{
    return renderComponents;
}

InputState& GameState::InputState()
{
    return inputState;
}

EntitySystem& GameState::EntitySystem()
{
    return entitySystem;
}

IdentitySystem& GameState::IdentitySystem()
{
    return identitySystem;
}

HealthSystem& GameState::HealthSystem()
{
    return healthSystem;
}

CollisionSystem& GameState::CollisionSystem()
{
    return collisionSystem;
}

ScriptSystem& GameState::ScriptSystem()
{
    return scriptSystem;
}

RenderSystem& GameState::RenderSystem()
{
    return renderSystem;
}

InterfaceSystem& GameState::InterfaceSystem()
{
    return interfaceSystem;
}

SpawnSystem& GameState::SpawnSystem()
{
    return spawnSystem;
}

ProgressSystem& GameState::ProgressSystem()
{
    return progressSystem;
}