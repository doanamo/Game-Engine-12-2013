#pragma once

#include "Precompiled.hpp"

//
// Spawn System
//

class SpawnSystem
{
public:
    typedef void SpawnFunction(const glm::vec2& position);

public:
    SpawnSystem();
    ~SpawnSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

    void SetSpawnArea(const glm::vec4& area);

    void AddSpawn(SpawnFunction* function, float minDelay, float maxDelay);
    void ResetSpawns();

private:
    struct SpawnDefinition
    {
        SpawnFunction* function;
        float minDelay, maxDelay;
        float currentDelay;
    };

    typedef std::vector<SpawnDefinition> SpawnList;

private:
    glm::vec4 m_spawnArea;
    SpawnList m_spawnList;
};
