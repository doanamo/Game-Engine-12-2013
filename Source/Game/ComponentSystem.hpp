#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class EntitySystem;
class Entity;

//
// Component System
//

class ComponentSystem
{
private:
    // Friend declarations.
    friend EntitySystem;

protected:
    ComponentSystem();

public:
    virtual ~ComponentSystem();

    virtual void PrepareProcessing() { }
    virtual void Process(Entity* entity) = 0;
    virtual void FinishProcessing() { }

    float GetTimeDelta() const;

    // Gets the entity system that owns this system.
    EntitySystem*       GetEntitySystem();
    const EntitySystem* GetEntitySystem() const;

private:
    // Owner of this component system.
    EntitySystem* m_entitySystem;
};
