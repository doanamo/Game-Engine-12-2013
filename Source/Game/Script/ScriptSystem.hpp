#pragma once

#include "Precompiled.hpp"
#include "Common/Receiver.hpp"
#include "Game/Event/EventDefinitions.hpp"

// Forward declarations.
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

    bool Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, ComponentSystem* componentSystem);
    void Cleanup();

    void Update(float timeDelta);

public:
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
    Receiver<GameEvent::EntityDamaged> m_receiverEntityDamaged;
    Receiver<GameEvent::EntityHealed> m_receiverEntityHealed;
    Receiver<GameEvent::EntityCollision> m_receiverEntityCollsion;
};
