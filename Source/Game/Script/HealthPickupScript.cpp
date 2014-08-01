#include "Precompiled.hpp"
#include "HealthPickupScript.hpp"

#include "Game/GameGlobal.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Health/HealthSystem.hpp"

HealthPickupScript::HealthPickupScript(int healValue) :
    m_healValue(healValue)
{
}

void HealthPickupScript::OnCollision(const CollisionObject& self, const CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Heal the collided entity.
    if(m_healValue > 0)
    {
        GameState::GetHealthSystem().Heal(other.entity, m_healValue);
    }
    
    // Destroy itself.
    GameState::GetEntitySystem().DestroyEntity(self.entity);
}
