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

    void AddSpawn(Lua::LuaRef function, const glm::vec4& area, float minDelay, float maxDelay);
    void RemoveAllSpawns();

private:
    struct SpawnDefinition
    {
        SpawnDefinition(Lua::LuaRef function, const glm::vec4& area, float minDelay, float maxDelay) :
            function(function), area(area), minDelay(minDelay), maxDelay(maxDelay)
        {
        }

        Lua::LuaRef function;
        glm::vec4 area;
        float minDelay;
        float maxDelay;
        float currentDelay;
    };

    typedef std::vector<SpawnDefinition> SpawnList;

private:
    SpawnList m_spawnList;
};
