#pragma once

#include "Precompiled.hpp"
#include "BaseFrame.hpp"

#include "EntityHandle.hpp"
#include "HealthBar.hpp"

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
    EntityHandle m_playerEntity;
    HealthBar m_playerHealthBar;

    bool m_initialized;
};
