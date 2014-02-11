#include "Precompiled.hpp"
#include "SpawnerScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "TransformComponent.hpp"

SpawnerScript::SpawnerScript() :
    m_spawnedEntity(),
    m_respawnTime(0.0f)
{
}

void SpawnerScript::OnCreate(EntityHandle entity)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Create an enemy entity.
    m_spawnedEntity = Game::CreateEnemy(transform->GetPosition());
}

void SpawnerScript::OnUpdate(EntityHandle entity, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Check if spawned entity is alive.
    if(!Game::EntitySystem().IsHandleValid(m_spawnedEntity))
    {
        // Create an enitity after a delay.
        if(m_respawnTime >= 3.0f)
        {
            // Create an enemy entity.
            m_spawnedEntity = Game::CreateEnemy(transform->GetPosition());

            // Reset respawn timer.
            m_respawnTime = 0.0f;
        }
        else
        {
            // Increment respawn timer.
            m_respawnTime += timeDelta;
        }
    }
}
