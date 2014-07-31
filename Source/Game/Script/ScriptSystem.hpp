#pragma once

#include "Precompiled.hpp"
#include "Common/Receiver.hpp"
#include "Game/Event/EventDefinitions.hpp"

// Forward declarations.
class EventSystem;

//
// Script System
//

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize(EventSystem* eventSystem);
    void Cleanup();

    void Update(float timeDelta);

public:
    void OnEntityDamagedEvent(const GameEvent::EntityDamaged& event);
    void OnEntityHealedEvent(const GameEvent::EntityHealed& event);
    void OnEntityCollisionEvent(const GameEvent::EntityCollision& event);

private:
    // Event receivers.
    Receiver<GameEvent::EntityDamaged> m_receiverEntityDamaged;
    Receiver<GameEvent::EntityHealed> m_receiverEntityHealed;
    Receiver<GameEvent::EntityCollision> m_receiverEntityCollsion;
};
