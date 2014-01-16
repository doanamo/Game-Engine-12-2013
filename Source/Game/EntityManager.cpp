#include "Precompiled.hpp"
#include "EntityManager.hpp"
#include "EntitySystem.hpp"

EntityManager::EntityManager() :
    m_entitySystem(nullptr)
{
}

EntityManager::~EntityManager()
{
}

EntitySystem* EntityManager::GetEntitySystem()
{
    return m_entitySystem;
}

const EntitySystem* EntityManager::GetEntitySystem() const
{
    return m_entitySystem;
}
