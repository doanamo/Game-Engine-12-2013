#pragma once

#include "Precompiled.hpp"
#include "Game/Entity/EntityHandle.hpp"

// Forward declarations.
class EventSystem;

//
// Health System
//

class HealthSystem
{
public:
    HealthSystem();
    ~HealthSystem();

    bool Initialize(EventSystem* eventSystem);
    void Cleanup();

    void Damage(EntityHandle entity, int value);
    void Heal(EntityHandle entity, int value);

private:
    // Event system.
    EventSystem* m_eventSystem;
};
