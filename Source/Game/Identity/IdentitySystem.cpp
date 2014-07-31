#include "Precompiled.hpp"
#include "IdentitySystem.hpp"

namespace
{
    // Invalid name constant.
    const char* InvalidName = "";
}

IdentitySystem::IdentitySystem() :
    m_initialized(false)
{
}

IdentitySystem::~IdentitySystem()
{
    Cleanup();
}

bool IdentitySystem::Initialize()
{
    Cleanup();

    // Success!
    return m_initialized = true;
}

void IdentitySystem::Cleanup()
{
    m_initialized = false;

    ClearContainer(m_names);
}

bool IdentitySystem::SetEntityName(const EntityHandle& entity, std::string name)
{
    if(!m_initialized)
        return false;

    // Remove existing entity from the map (make it anonymous). 
    auto it = m_names.right.find(entity);

    if(it != m_names.right.end())
    {
        // Check if it already has the same name.
        if(it->second == name)
            return true;

        // Remove entity from the map.
        m_names.right.erase(it);
    }

    // Insert entity with a new name.
    if(!name.empty())
    {
        auto result = m_names.left.insert(std::make_pair(name, entity));
        return result.second;
    }

    return true;
}

std::string IdentitySystem::GetEntityName(const EntityHandle& entity) const
{
    if(!m_initialized)
        return InvalidName;

    // Find entity name.
    auto it = m_names.right.find(entity);

    if(it != m_names.right.end())
    {
        // Return entity name.
        return it->second;
    }
    else
    {
        // Entity doesn't have a name (is anonymous).
        return InvalidName;
    }
}

EntityHandle IdentitySystem::GetEntityByName(std::string name) const
{
    if(!m_initialized)
        return EntityHandle();

    // Find entity by name.
    auto it = m_names.left.find(name);

    if(it != m_names.left.end())
    {
        // Return entity refered by this name.
        return it->second;
    }
    else
    {
        // Return an invalid entity.
        return EntityHandle();
    }
}

void IdentitySystem::OnDestroyEntity(const EntityHandle& entity)
{
    if(!m_initialized)
        return;

    // Remove entity from the name map.
    auto result = m_names.right.erase(entity);
    assert(result == 0 || result == 1);
}
