#pragma once

#include "Precompiled.hpp"

#include "Common/Receiver.hpp"
#include "Game/Entity/EntityHandle.hpp"
#include "Game/Event/EventDefinitions.hpp"

// Forward declarations.
class Services;
class EventSystem;

//
// Identity System
//

class IdentitySystem
{
private:
    typedef boost::bimap<
        boost::bimaps::unordered_set_of<std::string>,
        boost::bimaps::unordered_set_of<EntityHandle>
    > EntityNameMap;

public:
    IdentitySystem();
    ~IdentitySystem();

    bool Initialize(const Services& services);
    void Cleanup();

    bool SetEntityName(const EntityHandle& entity, std::string name);
    std::string GetEntityName(const EntityHandle& entity) const;
    EntityHandle GetEntityByName(std::string name) const;

private:
    void OnEntityDestroyedEvent(const GameEvent::EntityDestroyed& event);

private:
    // System state.
    bool m_initialized;

    // Event system.
    EventSystem* m_eventSystem;

    // List of named entities.
    EntityNameMap m_names;

    // Event receivers.
    Receiver<GameEvent::EntityDestroyed> m_receiverEntityDestroyed;
};
