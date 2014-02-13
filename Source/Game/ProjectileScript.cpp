#include "Precompiled.hpp"
#include "ProjectileScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "HealthSystem.hpp"
#include "CollisionSystem.hpp"

#include "TransformComponent.hpp"
#include "CollisionComponent.hpp"

ProjectileScript::ProjectileScript(const glm::vec2& direction, float speed) :
    m_direction(direction),
    m_speed(speed)
{
}

void ProjectileScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    // Move entity to the right.
    glm::vec2 position = transform->GetPosition();
    position += m_direction * m_speed * timeDelta;
    transform->SetPosition(position);
}

void ProjectileScript::OnCollision(CollisionObject& self, CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Apply damage to target entity.
    Game::HealthSystem().Damage(other.entity, 10);

    // Destroy itself.
    Game::EntitySystem().DestroyEntity(self.entity);
}
