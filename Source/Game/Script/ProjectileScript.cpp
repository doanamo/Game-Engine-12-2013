#include "Precompiled.hpp"
#include "ProjectileScript.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Health/HealthSystem.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Collision/CollisionComponent.hpp"

ProjectileScript::ProjectileScript() 
{
}

void ProjectileScript::OnCollision(const CollisionObject& self, const CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Apply damage to target entity.
    GameState::GetHealthSystem().Damage(other.entity, 10);

    // Destroy itself.
    GameState::GetEntitySystem().DestroyEntity(self.entity);
}
