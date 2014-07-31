#include "Precompiled.hpp"
#include "GameState.hpp"

#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
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

//
// State Data
//

namespace
{
    bool isInitialized = false;

    // Input state.
    InputState inputState;

    // Core systems.
    EntitySystem    entitySystem;
    ComponentSystem componentSystem; 

    // Component systems.
    IdentitySystem  identitySystem;
    HealthSystem    healthSystem;
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;
    InterfaceSystem interfaceSystem;

    // Game systems.
    SpawnSystem     spawnSystem;
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
    // Core Systems
    //

    // Initialize the entity system.
    if(!entitySystem.Initialize())
        return false;

    // Initialize the component system.
    if(!componentSystem.Initialize(&entitySystem))
        return false;

    // Create component pools.
    componentSystem.CreatePool<TransformComponent>();
    componentSystem.CreatePool<InputComponent>();
    componentSystem.CreatePool<HealthComponent>();
    componentSystem.CreatePool<CollisionComponent>();
    componentSystem.CreatePool<ScriptComponent>();
    componentSystem.CreatePool<RenderComponent>();

    //
    // Component Systems
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

    healthSystem.SubscribeReceiver(scriptSystem.GetEntityDamagedReceiver());
    healthSystem.SubscribeReceiver(scriptSystem.GetEntityHealedReceiver());
    collisionSystem.SubscribeReceiver(scriptSystem.GetEntityCollisionReceiver());

    // Initialize the render system.
    if(!renderSystem.Initialize(64))
        return false;

    // Initialize the interface system.
    if(!interfaceSystem.Initialize())
        return false;

    healthSystem.SubscribeReceiver(interfaceSystem.GetEntityDamagedReceiver());
    healthSystem.SubscribeReceiver(interfaceSystem.GetEntityHealedReceiver());

    //
    // Game Systems
    //

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

    // Entities must be destroyed first, then other
    // systems can be destroyed in a regular order.
    entitySystem.DestroyAllEntities();

    //
    // Game System
    //

    spawnSystem.Cleanup();

    //
    // Component Systems
    //

    interfaceSystem.Cleanup();
    renderSystem.Cleanup();
    scriptSystem.Cleanup();
    collisionSystem.Cleanup();
    healthSystem.Cleanup();
    identitySystem.Cleanup();

    //
    // Core Systems
    //

    entitySystem.Cleanup();
    componentSystem.Cleanup();

    //
    // Input State
    //

    inputState.Cleanup();

    isInitialized = false;
}

//
// State Accessors
//

bool GameState::IsInitialized()
{
    return isInitialized;
}

InputState& GameState::GetInputState()
{
    return inputState;
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
