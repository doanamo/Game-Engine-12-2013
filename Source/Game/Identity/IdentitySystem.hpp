#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"
#include "Game/Entity/EntitySubscriber.hpp"

//
// Identity System
//

class IdentitySystem : public EntitySubscriber
{
private:
    typedef boost::bimap<
        boost::bimaps::unordered_set_of<std::string>,
        boost::bimaps::unordered_set_of<EntityHandle>
    > EntityNameMap;

public:
    IdentitySystem();
    ~IdentitySystem();

    bool Initialize();
    void Cleanup();

    bool SetEntityName(const EntityHandle& entity, std::string name);
    std::string GetEntityName(const EntityHandle& entity) const;
    EntityHandle GetEntityByName(std::string name) const;

private:
    void OnDestroyEntity(const EntityHandle& entity);

private:
    bool m_initialized;

    // List of named entities.
    EntityNameMap m_names;
};
