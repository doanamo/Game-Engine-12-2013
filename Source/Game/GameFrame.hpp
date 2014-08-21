#pragma once

#include "Precompiled.hpp"
#include "System/BaseFrame.hpp"

#include "Game/GameState.hpp"

//
// Game Frame
//

class GameFrame : public BaseFrame
{
public:
    GameFrame();
    ~GameFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float timeDelta);
    void Draw();

    bool IsInitialized() const
    {
        return m_initialized;
    }

private:
    bool m_initialized;

    // Game state.
    GameState m_gameState;

    // Player dead state.
    bool m_playerDead;
    float m_deadTimer;
};
