#include "Precompiled.hpp"
#include "GameContext.hpp"

#include "MainContext.hpp"
#include "System/BaseFrame.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/GameFrame.hpp"
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
// Context Data
//

namespace
{
    bool isInitialized;

    InputState inputState;

    EntitySystem    entitySystem;
    IdentitySystem  identitySystem;
    HealthSystem    healthSystem;
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;
    InterfaceSystem interfaceSystem;

    ComponentPool<TransformComponent> transformComponents;
    ComponentPool<InputComponent>     inputComponents;
    ComponentPool<HealthComponent>    healthComponents;
    ComponentPool<CollisionComponent> collisionComponents;
    ComponentPool<ScriptComponent>    scriptComponents;
    ComponentPool<RenderComponent>    renderComponents;

    SpawnSystem    spawnSystem;
    ProgressSystem progressSystem;

    StateMachine<BaseFrame*> frameState;
    MenuFrame                menuFrame;
    GameFrame                gameFrame;
}

//
// Context Methods
//

bool Game::Initialize()
{
    assert(!isInitialized);

    // Check if the main context is initialized.
    if(!Main::IsInitialized())
        return false;

    //
    // Scope Guard
    //

    // Emergency cleanup call on failure.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!isInitialized)
        {
            Cleanup();
        }
    });

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

void Game::Cleanup()
{
    if(!isInitialized)
        return;

    Log() << "Cleaning up the game context...";

    //
    // Frame State
    //

    frameState.Cleanup();

    //
    // Input State
    //

    inputState.Cleanup();

    //
    // Game Systems
    //

    spawnSystem.Cleanup();
    progressSystem.Cleanup();

    //
    // Entity Systems
    //
    
    healthSystem.Cleanup();
    collisionSystem.Cleanup();
    scriptSystem.Cleanup();
    renderSystem.Cleanup();
    interfaceSystem.Cleanup();

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
// Context Accessors
//

InputState& Game::InputState()
{
    return inputState;
}

EntitySystem& Game::EntitySystem()
{
    return entitySystem;
}

IdentitySystem& Game::IdentitySystem()
{
    return identitySystem;
}

HealthSystem& Game::HealthSystem()
{
    return healthSystem;
}

CollisionSystem& Game::CollisionSystem()
{
    return collisionSystem;
}

ScriptSystem& Game::ScriptSystem()
{
    return scriptSystem;
}

RenderSystem& Game::RenderSystem()
{
    return renderSystem;
}

InterfaceSystem& Game::InterfaceSystem()
{
    return interfaceSystem;
}

ComponentPool<TransformComponent>& Game::TransformComponents()
{
    return transformComponents;
}

ComponentPool<InputComponent>& Game::InputComponents()
{
    return inputComponents;
}

ComponentPool<CollisionComponent>& Game::CollisionComponents()
{
    return collisionComponents;
}

ComponentPool<HealthComponent>& Game::HealthComponents()
{
    return healthComponents;
}

ComponentPool<ScriptComponent>& Game::ScriptComponents()
{
    return scriptComponents;
}

ComponentPool<RenderComponent>& Game::RenderComponents()
{
    return renderComponents;
}

SpawnSystem& Game::SpawnSystem()
{
    return spawnSystem;
}

ProgressSystem& Game::ProgressSystem()
{
    return progressSystem;
}

StateMachine<BaseFrame*>& Game::FrameState()
{
    return frameState;
}

MenuFrame& Game::MenuFrame()
{
    return menuFrame;
}

GameFrame& Game::GameFrame()
{
    return gameFrame;
}
