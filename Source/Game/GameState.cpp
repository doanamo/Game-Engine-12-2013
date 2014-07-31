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

    // Game systems.
    EntitySystem    entitySystem;
    ComponentSystem componentSystem; 
    IdentitySystem  identitySystem;
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
    // Input State
    //

    // Initialize the input state.
    if(!inputState.Initialize())
        return false;

    //
    // Systems
    //

    // Initialize the entity system.
    if(!entitySystem.Initialize())
        return false;

    // Initialize the component system.
    if(!componentSystem.Initialize(&entitySystem))
        return false;

    // Declare component types.
    componentSystem.Declare<TransformComponent>();
    componentSystem.Declare<InputComponent>();
    componentSystem.Declare<HealthComponent>();
    componentSystem.Declare<CollisionComponent>();
    componentSystem.Declare<ScriptComponent>();
    componentSystem.Declare<RenderComponent>();

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
    identitySystem.Cleanup();
    componentSystem.Cleanup();
    entitySystem.Cleanup();

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
