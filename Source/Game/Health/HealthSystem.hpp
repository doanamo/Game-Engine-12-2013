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
    void SubscribeReceiver(const ReceiverSignature<HealthChangeEvent>& signature);

private:
    // Event dispatcher.
    Dispatcher<HealthChangeEvent> m_dispatcherHealthChange;
};
