#pragma once

#include "Precompiled.hpp"
#include "CollisionObject.hpp"

#include "Game/Entity/EntityHandle.hpp"

// Forward declarations.
class EventSystem;
class EntitySystem;
class ComponentSystem;

//
// Collision System
//

class CollisionSystem
{
public:
    // Special values.
    static const float Permanent;

    // Type declarations.
    typedef std::vector<CollisionObject> ObjectList;
    typedef std::pair<EntityHandle, EntityHandle> EntityPair;
    typedef std::unordered_multimap<EntityPair, float> DisabledList;

public:
    CollisionSystem();
    ~CollisionSystem();

    bool Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, ComponentSystem* componentSystem);
    void Cleanup();

    void Update(float timeDelta);

    void DisableCollisionResponse(EntityHandle sourceEntity, EntityHandle targetEntity, float duration = Permanent);

private:
    // System state.
    bool m_initialized;

    // Game systems.
    EventSystem*     m_eventSystem;
    EntitySystem*    m_entitySystem;
    ComponentSystem* m_componentSystem;

    // Intermediate collision objects.
    ObjectList m_objects;

    // Disabled collision responses.
    DisabledList m_disabled;
};
