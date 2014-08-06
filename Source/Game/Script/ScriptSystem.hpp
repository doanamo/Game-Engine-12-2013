#pragma once

#include "Precompiled.hpp"
#include "Common/Receiver.hpp"
#include "Game/Event/EventDefinitions.hpp"

// Forward declarations.
class Services;
class EventSystem;
class EntitySystem;
class ComponentSystem;

//
// Script System
//

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize(const Services& services);
    void Cleanup();

    void Update(float timeDelta);

public:
    void OnEntityCreatedEvent(const GameEvent::EntityCreated& event);
    void OnEntityDestroyedEvent(const GameEvent::EntityDestroyed& event);
    void OnEntityDamagedEvent(const GameEvent::EntityDamaged& event);
    void OnEntityHealedEvent(const GameEvent::EntityHealed& event);
    void OnEntityCollisionEvent(const GameEvent::EntityCollision& event);

private:
    // System state.
    bool m_initialized;

    // Game systems.
    EventSystem*     m_eventSystem;
    EntitySystem*    m_entitySystem;
    ComponentSystem* m_componentSystem;

    // Event receivers.
    Receiver<GameEvent::EntityCreated> m_receiverEntityCreated;
    Receiver<GameEvent::EntityDestroyed> m_receiverEntityDestroyed;
    Receiver<GameEvent::EntityDamaged> m_receiverEntityDamaged;
    Receiver<GameEvent::EntityHealed> m_receiverEntityHealed;
    Receiver<GameEvent::EntityCollision> m_receiverEntityCollsion;
};
