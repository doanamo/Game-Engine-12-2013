#include "Precompiled.hpp"
#include "GameContext.hpp"
#include "MainContext.hpp"

#include "Common/StateMachine.hpp"

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

#include "MenuFrame.hpp"
#include "GameFrame.hpp"

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

    StateMachine<BaseFrame> stateMachine;
    MenuFrame               frameMenu;
    GameFrame               frameGame;
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
    // Main Frame
    //

    // Initialize the main menu frame.
    if(!frameMenu.Initialize())
        return false;

    // Set this frame as current state.
    stateMachine.ChangeState(&frameMenu);

    //
    // Success
    //

    isInitialized = true;

    return true;
}

void Game::Cleanup()
{
    Log() << "Cleaning up the game context...";

    //
    // Input State
    //

    inputState.Cleanup();

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
    // Game Frames
    //

    frameGame.Cleanup();
    frameMenu.Cleanup();

    //
    // State Machine
    //

    stateMachine.Cleanup();

    //
    // Game
    //

    isInitialized = false;
}

//
// Context Accessors
//

StateMachine<BaseFrame>& Game::StateMachine()
{
    return stateMachine;
}

MenuFrame& Game::MenuFrame()
{
    return frameMenu;
}

GameFrame& Game::GameFrame()
{
    return frameGame;
}

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
