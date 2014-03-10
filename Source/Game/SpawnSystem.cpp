#include "Precompiled.hpp"
#include "SpawnSystem.hpp"

#include "GameFactory.hpp"

namespace
{
    std::mt19937 spawnRandom((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 coordRandom((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
}

SpawnSystem::SpawnSystem() :
    m_spawnArea(0.0f, 0.0f, 0.0f, 0.0f)
{
}

SpawnSystem::~SpawnSystem()
{
    Cleanup();
}

bool SpawnSystem::Initialize()
{
    Cleanup();

    return true;
}

void SpawnSystem::Cleanup()
{
    m_spawnArea = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    m_spawnList.clear();
}

void SpawnSystem::Update(float timeDelta)
{
    // Calculate spawn area size.
    glm::vec2 size;
    size.x = m_spawnArea.z - m_spawnArea.x;
    size.y = m_spawnArea.w - m_spawnArea.y;

    assert(size.x >= 0 && size.y >= 0);

    // Traverse the spawn definition list.
    for(auto& definition : m_spawnList)
    {
        // Update the current delay timer.
        definition.currentDelay = std::max(0.0f, definition.currentDelay - timeDelta);

        // Use definition to spawn a new entity.
        if(definition.currentDelay == 0)
        {
            // Pick a random point inside the spawn area.
            glm::vec2 position;
            position.x = m_spawnArea.x + std::uniform_real<float>(0.0f, size.x)(coordRandom);
            position.y = m_spawnArea.y + std::uniform_real<float>(0.0f, size.y)(coordRandom);

            // Call the spawn function.
            definition.function(position);

            // Set a new spawn delay.
            definition.currentDelay = std::uniform_real<float>(definition.minDelay, definition.maxDelay)(spawnRandom);
        }
    }
}

void SpawnSystem::SetSpawnArea(const glm::vec4& area)
{
    m_spawnArea = area;
}

void SpawnSystem::AddSpawn(SpawnFunction function, float minDelay, float maxDelay)
{
    // Create a spawn definition.
    SpawnDefinition definition;
    definition.function = function;
    definition.minDelay = minDelay;
    definition.maxDelay = maxDelay;

    // Roll the initial delay.
    definition.currentDelay = std::uniform_real<float>(minDelay, maxDelay)(spawnRandom);

    // Add a definition to the list.
    m_spawnList.push_back(definition);
}

void SpawnSystem::ResetSpawns()
{
    m_spawnList.clear();
}
