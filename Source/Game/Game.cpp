#include "Precompiled.hpp"
#include "Game.hpp"

#include "MenuFrame.hpp"
#include "GameFrame.hpp"
#include "Context.hpp"

//
// Context
//

namespace
{
    // Context state.
    bool isInitialized;

    // Game state.
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
    if(!Context::IsInitialized())
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
    // Menu Frame
    //

    // Initialize the main menu frame.
    if(!frameMenu.Initialize())
        return false;

    // Set this frame as default.
    stateMachine.SetCurrentState(&frameMenu);

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
