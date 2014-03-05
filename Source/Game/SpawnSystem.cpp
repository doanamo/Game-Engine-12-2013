#include "Precompiled.hpp"
#include "SpawnSystem.hpp"

#include "GameFactory.hpp"

namespace
{
    std::mt19937 spawnRandom((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 coordRandom((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
}

SpawnSystem::SpawnSystem() :
    m_spawnArea(0.0f, 0.0f, 0.0f, 0.0f),
    m_spawnDelay(1.0f)
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
}

void SpawnSystem::Update(float timeDelta)
{
    m_spawnDelay = std::max(0.0f, m_spawnDelay - timeDelta);

    if(m_spawnDelay == 0)
    {
        // Calculate spawn area size.
        glm::vec2 size;
        size.x = m_spawnArea.z - m_spawnArea.x;
        size.y = m_spawnArea.w - m_spawnArea.y;

        assert(size.x >= 0 && size.y >= 0);

        // Pick a random point inside the spawn area.
        glm::vec2 position;
        position.x = m_spawnArea.x + std::uniform_real<float>(0.0f, size.x)(coordRandom);
        position.y = m_spawnArea.y + std::uniform_real<float>(0.0f, size.y)(coordRandom);

        // Spawn an entity.
        Game::CreateEnemy(position);

        // Set new spawn delay.
        m_spawnDelay = std::uniform_real<float>(0.5f, 1.0f)(spawnRandom);
    }
}

void SpawnSystem::SetSpawnArea(const glm::vec4& area)
{
    m_spawnArea = area;
}
