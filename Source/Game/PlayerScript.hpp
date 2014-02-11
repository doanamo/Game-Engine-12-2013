#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Player Script
//

class PlayerScript : public Script
{
public:
    PlayerScript();

    void OnUpdate(EntityHandle entity, float timeDelta);

public:
    float m_shootTime;
};
