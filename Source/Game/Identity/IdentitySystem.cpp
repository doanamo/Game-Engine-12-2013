#include "Precompiled.hpp"
#include "IdentitySystem.hpp"

#include "Common/Services.hpp"
#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"

namespace
{
    // Invalid name constant.
    const char* InvalidName = "";
}

IdentitySystem::IdentitySystem() :
    m_initialized(false),
    m_eventSystem(nullptr)
{
}

IdentitySystem::~IdentitySystem()
{
    Cleanup();
}

void IdentitySystem::Cleanup()
{
    m_initialized = false;

    m_eventSystem = nullptr;

    ClearContainer(m_names);

    m_receiverEntityDestroyed.Cleanup();
}

bool IdentitySystem::Initialize(const Services& services)
{
    Cleanup();

    // Setup scope guard.
    SCOPE_GUARD_IF(!m_initialized, Cleanup());

    // Get required system.
    m_eventSystem = services.Get<EventSystem>();
    if(m_eventSystem == nullptr) return false;

    // Bind event receiver.
    m_receiverEntityDestroyed.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyedEvent>(this);

    // Subscribe to event receivers.
    m_eventSystem->Subscribe<GameEvent::EntityDestroyed>(m_receiverEntityDestroyed);

    // Success!
    return m_initialized = true;
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

void IdentitySystem::OnEntityDestroyedEvent(const GameEvent::EntityDestroyed& event)
{
    if(!m_initialized)
        return;

    // Remove entity from the name map.
    auto result = m_names.right.erase(event.entity);
    assert(result == 0 || result == 1);
}
