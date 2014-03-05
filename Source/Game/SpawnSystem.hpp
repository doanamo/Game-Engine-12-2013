#pragma once

#include "Precompiled.hpp"

//
// Spawn System
//

class SpawnSystem
{
public:
    SpawnSystem();
    ~SpawnSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

    void SetSpawnArea(const glm::vec4& area);

private:
    glm::vec4 m_spawnArea;
    float     m_spawnDelay;
};
