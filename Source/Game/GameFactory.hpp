#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"

//
// Game Factory
//

namespace GameFactory
{
    void         CreateBounds();
    EntityHandle CreatePlayer();
    EntityHandle CreateAsteroid(const glm::vec2& position);
    EntityHandle CreateEnemy(const glm::vec2& position);
    EntityHandle CreateProjectile(const glm::vec2& position, const glm::vec2& direction, float speed, uint32_t collisionMask);
    EntityHandle CreateHealthPickup(const glm::vec2& position, int heal);
    EntityHandle CreateSpawner(const glm::vec2& position);
}
