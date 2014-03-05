#include "Precompiled.hpp"
#include "HealthPickupScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"
#include "HealthSystem.hpp"

#include "TransformComponent.hpp"

HealthPickupScript::HealthPickupScript(int healValue) :
    m_healValue(healValue)
{
}

void HealthPickupScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    // Move the entity to the left.
    glm::vec2 position = transform->GetPosition();
    position.x -= 100.0f * timeDelta;
    transform->SetPosition(position);
}

void HealthPickupScript::OnCollision(CollisionObject& self, CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Heal the collided entity.
    if(m_healValue > 0)
    {
        Game::HealthSystem().Heal(other.entity, m_healValue);
    }
    
    // Destroy itself.
    Game::EntitySystem().DestroyEntity(self.entity);
}
