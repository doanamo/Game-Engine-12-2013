#pragma once

#include "Precompiled.hpp"

#include "System/BaseFrame.hpp"
#include "Graphics/ScreenSpace.hpp"

//
// Lose Frame
//

class LoseFrame : public BaseFrame
{
public:
    LoseFrame();
    ~LoseFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float timeDelta);
    void Draw();

private:
    void OnEnter();

private:
    bool m_initialized;

    // Menu screen space.
    ScreenSpace m_screenSpace;

    // Frame cooldown timer.
    float m_cooldownTimer;
};
