#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Enemy Script
//

class EnemyScript : public Script
{
public:
    EnemyScript();

    void OnUpdate(EntityHandle self, float timeDelta);
    void OnDamage(EntityHandle self, int value, bool alive);

private:
    float m_shootTime;
};
