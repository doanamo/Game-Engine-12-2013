#include "Precompiled.hpp"
#include "GameContext.hpp"
#include "MainContext.hpp"

#include "InputState.hpp"

#include "EntitySystem.hpp"
#include "HealthSystem.hpp"
#include "CollisionSystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

#include "TransformComponent.hpp"
#include "InputComponent.hpp"
#include "HealthComponent.hpp"
#include "CollisionComponent.hpp"
#include "ScriptComponent.hpp"
#include "RenderComponent.hpp"

#include "SpawnSystem.hpp"
#include "ProgressSystem.hpp"

#include "System/BaseFrame.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/GameFrame.hpp"

//
// Context Data
//

namespace
{
    bool isInitialized;

    InputState inputState;

    EntitySystem    entitySystem;
    HealthSystem    healthSystem;
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;

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
