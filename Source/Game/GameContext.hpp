#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"

// Forward declarations.
class BaseFrame;
class MenuFrame;
class GameFrame;

//
// Game Context
//

namespace Game
{
    // Context methods.
    bool Initialize();
    void Cleanup();

    // Context accessors.
    bool IsInitialized();

    StateMachine<BaseFrame>& StateMachine();
    MenuFrame&               MenuFrame();
    GameFrame&               GameFrame();
}
