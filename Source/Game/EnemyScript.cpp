#include "Precompiled.hpp"
#include "EnemyScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "CollisionTypes.hpp"
#include "TransformComponent.hpp"

EnemyScript::EnemyScript() :
    m_shootTime(1.0f)
{
}

void EnemyScript::OnUpdate(EntityHandle entity, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Shoot a projectile.
    m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

    if(m_shootTime == 0.0f)
    {
        // Create a projectile entity.
        Game::CreateProjectile(transform->GetPosition(), glm::vec2(-1.0f, 0.0f), 300.0f, CollisionTypes::Player);

        // Set a shooting delay.
        m_shootTime = 2.0f;
    }
}
