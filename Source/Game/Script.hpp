#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "CollisionSystem.hpp"

//
// Script
//

class Script
{
public:
    virtual void OnUpdate(EntityHandle entity, float timeDelta)
    {
    }

    virtual void OnCollision(CollisionObject& object, CollisionObject& other)
    {
    }
};
