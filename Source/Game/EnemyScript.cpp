#include "Precompiled.hpp"
#include "EnemyScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "CollisionTypes.hpp"
#include "TransformComponent.hpp"

namespace
{
    // Random number generator.
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator((unsigned int)seed);
    std::uniform_real<float> random(1.0f, 3.0f);
}

EnemyScript::EnemyScript() :
    m_shootTime(random(generator))
{
}

void EnemyScript::OnUpdate(EntityHandle entity, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Move to the left side of the screen.
    glm::vec2 position = transform->GetPosition();
    position.x -= 200.0f * timeDelta;
    transform->SetPosition(position);

    // Shoot a projectile.
    m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

    if(m_shootTime == 0.0f)
    {
        // Create a projectile entity.
        Game::CreateProjectile(transform->GetPosition(), glm::vec2(-1.0f, 0.0f), 400.0f, CollisionTypes::Player);

        // Set a shooting delay.
        m_shootTime = random(generator);
    }
}
