#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Health Component
//

class HealthComponent : public Component
{
public:
    HealthComponent();
    ~HealthComponent();

    void SetMaximumHealth(int value)
    {
        m_maximumHealth = value;
    }

    void SetCurrentHealth(int value)
    {
        m_currentHealth = value;
    }

    int GetMaximumHealth() const
    {
        return m_maximumHealth;
    }

    int GetCurrentHealth() const
    {
        return m_currentHealth;
    }

    bool IsAlive() const
    {
        return m_currentHealth != 0;
    }

    bool IsDead() const
    {
        return m_currentHealth == 0;
    }

private:
    int m_maximumHealth;
    int m_currentHealth;
};
