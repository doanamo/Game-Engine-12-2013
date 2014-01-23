#include "Precompiled.hpp"
#include "GameContext.hpp"
#include "MainContext.hpp"

#include "Common/StateMachine.hpp"

#include "InputState.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

#include "Transform.hpp"
#include "Collision.hpp"
#include "Input.hpp"
#include "Script.hpp"
#include "Render.hpp"

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
    CollisionSystem collisionSystem;
    ScriptSystem    scriptSystem;
    RenderSystem    renderSystem;

    ComponentPool<Transform> transformComponents;
    ComponentPool<Collision> collisionComponents;
    ComponentPool<Input>     inputComponents;
    ComponentPool<Script>    scriptComponents;
    ComponentPool<Render>    renderComponents;

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

    // Initialize the collision system.
    if(!collisionSystem.Initialize())
        return false;

    // Initialize the script system.
    if(!scriptSystem.Initialize())
        return false;

    // Initialize the render system.
    if(!renderSystem.Initialize())
        return false;

    //
    // Component Pools
    //

    // Subscribe component pools to the entity system.
    entitySystem.RegisterSubscriber(&transformComponents);
    entitySystem.RegisterSubscriber(&collisionComponents);
    entitySystem.RegisterSubscriber(&inputComponents);
    entitySystem.RegisterSubscriber(&scriptComponents);
    entitySystem.RegisterSubscriber(&renderComponents);

    //
    // Menu Frame
    //

    // Initialize the main menu frame.
    if(!frameMenu.Initialize())
        return false;

    // Set this frame as current state.
    stateMachine.ChangeState(&frameMenu);

    //
    // Game Frame
    //

    // Initialize the game frame.
    if(!frameGame.Initialize())
        return false;

    // Success!
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
    
    collisionSystem.Cleanup();
    scriptSystem.Cleanup();
    renderSystem.Cleanup();

    entitySystem.Cleanup();

    //
    // Component Pools
    //

    transformComponents.Cleanup();
    collisionComponents.Cleanup();
    inputComponents.Cleanup();
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

ComponentPool<Transform>& Game::TransformComponents()
{
    return transformComponents;
}

ComponentPool<Collision>& Game::CollisionComponents()
{
    return collisionComponents;
}

ComponentPool<Input>& Game::InputComponents()
{
    return inputComponents;
}

ComponentPool<Script>& Game::ScriptComponents()
{
    return scriptComponents;
}

ComponentPool<Render>& Game::RenderComponents()
{
    return renderComponents;
}
