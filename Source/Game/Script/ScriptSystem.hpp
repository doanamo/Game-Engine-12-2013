#pragma once

#include "Precompiled.hpp"

#include "Common/Receiver.hpp"
#include "Game/GameEvents.hpp"

//
// Script System
//

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

public:
    ReceiverSignature<GameEvent::EntityDamaged> GetEntityDamagedReceiver();
    ReceiverSignature<GameEvent::EntityHealed> GetEntityHealedReceiver();

    void OnEntityDamagedEvent(const GameEvent::EntityDamaged& event);
    void OnEntityHealedEvent(const GameEvent::EntityHealed& event);

private:
    // Event receivers.
    Receiver<GameEvent::EntityDamaged> m_receiverEntityDamaged;
    Receiver<GameEvent::EntityHealed> m_receiverEntityHealed;
};
