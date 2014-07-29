#include "Precompiled.hpp"
#include "GameContext.hpp"

#include "MainContext.hpp"
#include "System/BaseFrame.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/GameFrame.hpp"
#include "Game/LoseFrame.hpp"

//
// Context Data
//

namespace
{
    bool isInitialized = false;

    StateMachine<BaseFrame*> frameState;

    MenuFrame menuFrame;
    GameFrame gameFrame;
    LoseFrame loseFrame;
}

//
// Context Methods
//

bool GameContext::Initialize()
{
    assert(!isInitialized);

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
    // Success
    //

    isInitialized = true;

    return true;
}

void GameContext::Cleanup()
{
    if(!isInitialized)
        return;

    Log() << "Cleaning up the game context...";

    //
    // Frame State
    //

    frameState.Cleanup();

    menuFrame.Cleanup();
    gameFrame.Cleanup();
    loseFrame.Cleanup();

    //
    // Game
    //

    isInitialized = false;
}

//
// Context Accessors
//

bool GameContext::IsInitialized()
{
    return isInitialized;
}

StateMachine<BaseFrame*>& GameContext::GetFrameState()
{
    return frameState;
}

MenuFrame& GameContext::GetMenuFrame()
{
    return menuFrame;
}

GameFrame& GameContext::GetGameFrame()
{
    return gameFrame;
}

LoseFrame& GameContext::GetLoseFrame()
{
    return loseFrame;
}
