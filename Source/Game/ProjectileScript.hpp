#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Projectile Script
//

class ProjectileScript : public Script
{
public:
    ProjectileScript(const glm::vec2& direction, float speed);

    void OnUpdate(EntityHandle entity, float timeDelta);
    void OnCollision(CollisionObject& self, CollisionObject& other);

private:
    glm::vec2 m_direction;
    float     m_speed;
};
