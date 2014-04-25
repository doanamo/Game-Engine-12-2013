#pragma once

#include "Precompiled.hpp"

//
// Entity Subscriber
//

class EntitySubscriber
{
public:
    // Called on creating an entity.
    virtual void OnCreateEntity(const EntityHandle& entity) { }

    // Called on destroying an entity.
    virtual void OnDestroyEntity(const EntityHandle& entity) { }
};
