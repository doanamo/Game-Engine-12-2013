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

    void OnUpdate(EntityHandle entity, float timeDelta);

private:
    float m_shootTime;
};
