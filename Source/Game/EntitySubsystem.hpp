#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class EntitySystem;
class Entity;

//
// Entity Subsystem
//

class EntitySubsystem
{
private:
    // Friend declarations.
    friend EntitySystem;

protected:
    EntitySubsystem();

public:
    virtual ~EntitySubsystem();

    // Entity processing.
    virtual void PrepareProcessing() { }
    virtual void Process(Entity* entity) = 0;
    virtual void FinishProcessing() { }

    // Gets the entity system that owns this subsystem.
    EntitySystem*       GetEntitySystem();
    const EntitySystem* GetEntitySystem() const;

private:
    // Called when the entity system creates an entity.
    virtual void OnCreateEntity(Entity* entity) { }

    // Called when the entity system destroys an entity.
    virtual void OnDestroyEntity(Entity* entity) { }

private:
    // Owner of this entity subsystem.
    EntitySystem* m_entitySystem;
};
