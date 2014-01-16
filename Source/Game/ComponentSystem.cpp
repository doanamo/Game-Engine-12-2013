#include "Precompiled.hpp"
#include "ComponentSystem.hpp"
#include "EntitySystem.hpp"

ComponentSystem::ComponentSystem() :
    m_entitySystem(nullptr)
{
}

ComponentSystem::~ComponentSystem()
{
}

float ComponentSystem::GetTimeDelta() const
{
    if(m_entitySystem)
    {
        return m_entitySystem->GetTimeDelta();
    }
    else
    {
        return 0.0f;
    }
}

EntitySystem* ComponentSystem::GetEntitySystem()
{
    return m_entitySystem;
}

const EntitySystem* ComponentSystem::GetEntitySystem() const
{
    return m_entitySystem;
}
