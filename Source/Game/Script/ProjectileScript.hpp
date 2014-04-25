#pragma once

#include "Precompiled.hpp"
#include "Script.hpp"

//
// Projectile Script
//

class ProjectileScript : public Script
{
public:
    ProjectileScript();

    void OnCollision(CollisionObject& self, CollisionObject& other);
};
