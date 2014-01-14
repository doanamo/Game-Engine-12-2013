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
    // Game context methods.
    bool Initialize();
    void Cleanup();

    // Game context accessors.
    bool IsInitialized();

    StateMachine<BaseFrame>& StateMachine();
    MenuFrame&               MenuFrame();
    GameFrame&               GameFrame();
}
