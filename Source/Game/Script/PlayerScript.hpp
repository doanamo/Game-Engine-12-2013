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

    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);

public:
    float m_shootTime;
};
