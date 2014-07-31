#include "Precompiled.hpp"
#include "GameState.hpp"

#include "Game/Component/ComponentCollection.hpp"
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

    // Component collection.
    ComponentCollection componentCollection;

    // Input state.
    InputState inputState;

    // Core systems.
    EntitySystem    entitySystem;

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
    // Component Pools
    //

    // Subscribe component pools to the entity system.
    entitySystem.RegisterSubscriber(&transformComponents);
    entitySystem.RegisterSubscriber(&inputComponents);
    entitySystem.RegisterSubscriber(&healthComponents);
    entitySystem.RegisterSubscriber(&collisionComponents);
    entitySystem.RegisterSubscriber(&scriptComponents);
    entitySystem.RegisterSubscriber(&renderComponents);

    // Add pools to the component collection.
    componentCollection.Register(&transformComponents);
    componentCollection.Register(&inputComponents);
    componentCollection.Register(&healthComponents);
    componentCollection.Register(&collisionComponents);
    componentCollection.Register(&scriptComponents);
    componentCollection.Register(&renderComponents);

    //
    // Game Systems
    //

    // Initialize the spawn system.
    if(!spawnSystem.Initialize())
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
    // Component Pools
    //

    componentCollection.Cleanup();

    transformComponents.Cleanup();
    inputComponents.Cleanup();
    healthComponents.Cleanup();
    collisionComponents.Cleanup();
    scriptComponents.Cleanup();
    renderComponents.Cleanup();

    //
    // Core Systems
    //

    entitySystem.Cleanup();

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

ComponentPool<TransformComponent>& GameState::GetTransformComponents()
{
    return transformComponents;
}

ComponentPool<InputComponent>& GameState::GetInputComponents()
{
    return inputComponents;
}

ComponentPool<CollisionComponent>& GameState::GetCollisionComponents()
{
    return collisionComponents;
}

ComponentPool<HealthComponent>& GameState::GetHealthComponents()
{
    return healthComponents;
}

ComponentPool<ScriptComponent>& GameState::GetScriptComponents()
{
    return scriptComponents;
}

ComponentPool<RenderComponent>& GameState::GetRenderComponents()
{
    return renderComponents;
}

ComponentCollection& GameState::GetComponentCollection()
{
    return componentCollection;
}

InputState& GameState::GetInputState()
{
    return inputState;
}

EntitySystem& GameState::GetEntitySystem()
{
    return entitySystem;
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
