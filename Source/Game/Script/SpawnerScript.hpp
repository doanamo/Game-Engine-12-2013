#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Spawner Script
//

class SpawnerScript : public Script
{
public:
    SpawnerScript();

    void OnUpdate(EntityHandle self, float timeDelta);

private:
    float m_respawnTime;
};
