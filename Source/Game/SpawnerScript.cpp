#include "Precompiled.hpp"
#include "SpawnerScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "TransformComponent.hpp"

namespace
{
    // Random number generator.
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator((unsigned int)seed);
    std::uniform_real<float> random(1.0f, 6.0);
}

SpawnerScript::SpawnerScript() :
    m_respawnTime(random(generator))
{
}

void SpawnerScript::OnUpdate(EntityHandle entity, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Create an enitity after a delay.
    m_respawnTime = std::max(0.0f, m_respawnTime - timeDelta);

    if(m_respawnTime == 0.0f)
    {
        // Create an enemy entity.
        Game::CreateEnemy(transform->GetPosition());

        // Reset respawn timer.
        m_respawnTime = random(generator);
    }
}
