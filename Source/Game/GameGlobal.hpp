#pragma once

#include "Precompiled.hpp"

#include "Common/StateMachine.hpp"

// Forward declarations.
class BaseFrame;
class MenuFrame;
class GameFrame;
class LoseFrame;

//
// Game Global
//

namespace GameContext
{
    // Global methods.
    bool Initialize();
    void Cleanup();

    // Global accessors.
    bool IsInitialized();

    StateMachine<BaseFrame*>& GetFrameState();

    MenuFrame& GetMenuFrame();
    GameFrame& GetGameFrame();
    LoseFrame& GetLoseFrame();
}
