#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"

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

    virtual void OnCollision(const CollisionObject& self, const CollisionObject& other)
    {
    }
};
