#pragma once

#include "Precompiled.hpp"

// Forward declaration.
class EntitySystem;
class Entity;

//
// Entity Manager
//

class EntityManager
{
private:
    // Friend declarations.
    friend EntitySystem;

protected:
    EntityManager();

public:
    virtual ~EntityManager();

    // Gets the entity system that owns this manager.
    EntitySystem*       GetEntitySystem();
    const EntitySystem* GetEntitySystem() const;

private:
    // Called when the entity system creates an entity.
    virtual void OnCreateEntity(Entity* entity)
    {
    }

    // Called when the entity system destroys an entity.
    virtual void OnDestroyEntity(Entity* entity)
    {
    }

private:
    // Owner of this entity manager.
    EntitySystem* m_entitySystem;
};
