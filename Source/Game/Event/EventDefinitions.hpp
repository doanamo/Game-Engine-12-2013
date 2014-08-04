#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"
#include "Game/Collision/CollisionObject.hpp"
#include "Game/Health/HealthComponent.hpp"

//
// Game Events
//

namespace GameEvent
{
    // Called after entity has been just created.
    struct EntityCreated
    {
        EntityCreated(EntityHandle entity) :
            entity(entity)
        {
        }

        EntityHandle entity;
    };

    // Called when entity is just about to be destroyed.
    struct EntityDestroyed
    {
        EntityDestroyed(EntityHandle entity) :
            entity(entity)
        {
        }

        EntityHandle entity;
    };

    // Called when entity health changes.
    struct EntityHealth
    {
        EntityHealth() :
            entity(),
            component(nullptr)
        {
        }

        EntityHandle entity;
        HealthComponent* component;
    };

    // Called when entity has been damaged.
    struct EntityDamaged
    {
        EntityDamaged() :
            entity(),
            component(nullptr),
            value(0)
        {
        }

        EntityHandle entity;
        HealthComponent* component;
        int value;
    };

    // Called when entity has been healed.
    struct EntityHealed
    {
        EntityHealed() :
            entity(),
            component(nullptr),
            value(0)
        {
        }

        EntityHandle entity;
        HealthComponent* component;
        int value;
    };

    // Called when entity collides with another one.
    struct EntityCollision
    {
        EntityCollision(const CollisionObject& self, const CollisionObject& other) :
            self(self), other(other)
        {
        }

        const CollisionObject& self;
        const CollisionObject& other;
    };
}
