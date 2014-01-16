#include "Precompiled.hpp"
#include "EntitySubsystem.hpp"
#include "EntitySystem.hpp"

EntitySubsystem::EntitySubsystem() :
    m_entitySystem(nullptr)
{
}

EntitySubsystem::~EntitySubsystem()
{
}

EntitySystem* EntitySubsystem::GetEntitySystem()
{
    return m_entitySystem;
}

const EntitySystem* EntitySubsystem::GetEntitySystem() const
{
    return m_entitySystem;
}
