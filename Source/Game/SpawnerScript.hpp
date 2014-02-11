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

    void OnCreate(EntityHandle entity);
    void OnUpdate(EntityHandle entity, float timeDelta);

private:
    EntityHandle m_spawnedEntity;
    float        m_respawnTime;
};
