#include "Precompiled.hpp"
#include "SpawnerScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "TransformComponent.hpp"

namespace
{
    // Random number generator.
    std::random_device deviceRandom;
    std::mt19937 spawnRandom(deviceRandom());
    
}

SpawnerScript::SpawnerScript() :
    m_respawnTime(std::uniform_real<float>(3.0f, 6.0)(spawnRandom))
{
}

void SpawnerScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    // Create an enitity after a delay.
    m_respawnTime = std::max(0.0f, m_respawnTime - timeDelta);

    if(m_respawnTime == 0.0f)
    {
        // Create an enemy entity.
        Game::CreateEnemy(transform->GetPosition());

        // Reset respawn timer.
        m_respawnTime = std::uniform_real<float>(3.0f, 6.0)(spawnRandom);
    }
}
