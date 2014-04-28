#include "Precompiled.hpp"
#include "EnemyScript.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionDefinitions.hpp"

namespace
{
    // Random number generators.
    std::random_device deviceRandom;
    std::mt19937 shootRandom(deviceRandom());
    std::mt19937 pickupRandom(deviceRandom());
}

EnemyScript::EnemyScript() :
    m_shootTime(std::uniform_real<float>(0.5f, 2.0f)(shootRandom))
{
}

void EnemyScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = GameState::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    // Move to the left side of the screen.
    glm::vec2 position = transform->GetPosition();
    position.x -= 150.0f * timeDelta;
    transform->SetPosition(position);

    // Shoot a projectile.
    m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

    if(m_shootTime == 0.0f)
    {
        // Create a projectile entity.
        GameFactory::CreateProjectile(transform->GetPosition(), glm::vec2(-1.0f, 0.0f), 400.0f, CollisionTypes::Player | CollisionTypes::Environment);

        // Set a shooting delay.
        m_shootTime = std::uniform_real<float>(1.0f, 3.0f)(shootRandom);
    }
}

void EnemyScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        // Drop a health pickup.
        float roll = std::uniform_real<float>(0.0f, 1.0f)(pickupRandom);

        if(roll <= 0.1f)
        {
            // Get the transform component.
            TransformComponent* transform = GameState::TransformComponents().Lookup(self);

            // Create a health pickup entity.
            if(transform != nullptr)
            {
                GameFactory::CreateHealthPickup(transform->GetPosition(), 20);
            }
        }

        // Destroy the entity.
        GameState::EntitySystem().DestroyEntity(self);
    }
}
