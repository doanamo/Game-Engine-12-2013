#pragma once

#include "Precompiled.hpp"

#include "Common/StateMachine.hpp"

// Forward declarations.
class BaseFrame;
class MenuFrame;
class GameFrame;
class LoseFrame;

//
// Game Context
//

namespace GameContext
{
    // Context methods.
    bool Initialize();
    void Cleanup();

    // Context accessors.
    bool IsInitialized();

    StateMachine<BaseFrame*>& GetFrameState();

    MenuFrame& GetMenuFrame();
    GameFrame& GetGameFrame();
    LoseFrame& GetLoseFrame();
}
