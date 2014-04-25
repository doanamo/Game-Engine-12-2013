#include "Precompiled.hpp"
#include "HealthComponent.hpp"

HealthComponent::HealthComponent() :
    m_maximumHealth(100),
    m_currentHealth(100)
{
}

HealthComponent::~HealthComponent()
{
}
