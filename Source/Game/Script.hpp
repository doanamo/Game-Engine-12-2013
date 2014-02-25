#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

// Forward declarations.
struct CollisionObject;

//
// Script
//

class Script
{
public:
    virtual void OnCreate(EntityHandle self)
    {
    }

    virtual void OnUpdate(EntityHandle self, float timeDelta)
    {
    }

    virtual void OnDamage(EntityHandle self, int value, bool alive)
    {
    }

    virtual void OnHeal(EntityHandle self, int value)
    {
    }

    virtual void OnCollision(CollisionObject& self, CollisionObject& other)
    {
    }
};
