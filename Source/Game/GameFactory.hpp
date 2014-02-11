#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Game Factory
//

namespace Game
{
    EntityHandle CreatePlayer();
    EntityHandle CreateEnemy(const glm::vec2& position);
    EntityHandle CreateProjectile(const glm::vec2& position, const glm::vec2& direction, float speed, uint32_t collisionMask);
    EntityHandle CreateSpawner(const glm::vec2& position);
}
