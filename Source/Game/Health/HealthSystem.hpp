#pragma once

#include "Precompiled.hpp"

#include "Common/Dispatcher.hpp"
#include "Game/GameEvents.hpp"
#include "Game/Entity/EntityHandle.hpp"

//
// Health System
//

class HealthSystem
{
public:
    HealthSystem();
    ~HealthSystem();

    bool Initialize();
    void Cleanup();

    void Damage(EntityHandle entity, int value);
    void Heal(EntityHandle entity, int value);

public:
    void SubscribeReceiver(const ReceiverSignature<GameEvent::EntityHealth>& signature);
    void SubscribeReceiver(const ReceiverSignature<GameEvent::EntityDamaged>& signature);
    void SubscribeReceiver(const ReceiverSignature<GameEvent::EntityHealed>& signature);

private:
    // Event dispatchers.
    Dispatcher<GameEvent::EntityHealth> m_dispatcherEntityHealth;
    Dispatcher<GameEvent::EntityDamaged> m_dispatcherEntityDamaged;
    Dispatcher<GameEvent::EntityHealed> m_dispatcherEntityHealed;
};
