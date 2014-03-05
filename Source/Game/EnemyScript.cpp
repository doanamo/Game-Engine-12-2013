#include "Precompiled.hpp"
#include "EnemyScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"
#include "EntitySystem.hpp"

#include "CollisionTypes.hpp"
#include "TransformComponent.hpp"

namespace
{
    // Random number generator.
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator((unsigned int)seed);
    

    float random(float min, float max)
    {
        std::uniform_real<float> distribution(min, max);
        return distribution(generator);
    }
}

EnemyScript::EnemyScript() :
    m_shootTime(random(0.5f, 2.0f))
{
}

void EnemyScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(self);
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
        m_shootTime = random(1.0f, 3.0f);
    }
}

void EnemyScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        // Drop a health pickup.
        float roll = random(0.0f, 1.0f);

        if(roll <= 0.1f)
        {
            // Get the transform component.
            TransformComponent* transform = Game::TransformComponents().Lookup(self);

            // Create a health pickup entity.
            if(transform != nullptr)
            {
                Game::CreateHealthPickup(transform->GetPosition(), 20);
            }
        }

        // Destroy the entity.
        Game::EntitySystem().DestroyEntity(self);
    }
}
