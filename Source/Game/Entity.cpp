#include "Precompiled.hpp"
#include "Entity.hpp"
#include "EntitySystem.hpp"

Entity::Entity() :
    m_entitySystem(nullptr)
{
}

Entity::~Entity()
{
}

Entity::Entity(Entity&& other)
{
    m_entitySystem = other.m_entitySystem;
    other.m_entitySystem = nullptr;

    m_handle = other.m_handle;
    other.m_handle = EntityHandle();

    m_components = std::move(other.m_components);
}

Entity& Entity::operator=(Entity&& other)
{
    if(this != &other)
    {
        m_entitySystem = other.m_entitySystem;
        other.m_entitySystem = nullptr;

        m_handle = other.m_handle;
        other.m_handle = EntityHandle();

        m_components = std::move(other.m_components);
    }

    return *this;
}

void Entity::Destroy()
{
    m_entitySystem->DestroyEntity(m_handle);
}

EntitySystem* Entity::GetEntitySystem()
{
    return m_entitySystem;
}

const EntitySystem* Entity::GetEntitySystem() const
{
    return m_entitySystem;
}

const EntityHandle& Entity::GetHandle() const
{
    return m_handle;
}
