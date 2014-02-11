#include "Precompiled.hpp"
#include "ProjectileScript.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"

#include "TransformComponent.hpp"
#include "CollisionComponent.hpp"

ProjectileScript::ProjectileScript(const glm::vec2& direction, float speed) :
    m_direction(direction),
    m_speed(speed),
    m_lifeTime(0.0f)
{
}

void ProjectileScript::OnUpdate(EntityHandle entity, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    if(transform == nullptr) return;

    // Check if the projectile reached it's lifetime.
    m_lifeTime += timeDelta;

    if(m_lifeTime >= 1.0f)
    {
        Game::EntitySystem().DestroyEntity(entity);
        return;
    }

    // Move entity to the right.
    glm::vec2 position = transform->GetPosition();
    position += m_direction * m_speed * timeDelta;
    transform->SetPosition(position);
}

void ProjectileScript::OnCollision(CollisionObject& self, CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Disable collisions.
    self.collision->Disable();
    other.collision->Disable();

    // Destroy both entities.
    Game::EntitySystem().DestroyEntity(self.entity);
    Game::EntitySystem().DestroyEntity(other.entity);
}
