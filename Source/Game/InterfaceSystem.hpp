#pragma once

#include "Precompiled.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "ValueBar.hpp"

//
// Interface System
//

class InterfaceSystem
{
public:
    InterfaceSystem();
    ~InterfaceSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);
    void Draw();

private:
    bool m_initialized;

    // Screen space.
    ScreenSpace m_screenSpace;

    // Player health bar.
    ValueBar m_playerHealthBar;
};
