#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "EntitySubscriber.hpp"

//
// Entity System
//

class EntitySystem
{
public:
    // Handle flags.
    struct HandleFlags
    {
        enum Type
        {
            None    = 0,
            Active  = 1 << 0,
            Destroy = 1 << 1,
        };

        static const uint32_t Free = None; 
    };

    // Handle entry structure.
    struct HandleEntry
    {
        EntityHandle handle;
        int          nextFree;
        uint32_t     flags;
    };

    // Entity commands.
    struct EntityCommands
    {
        enum Type
        {
            Invalid,

            Create,
            Destroy,
        };
    };

    struct EntityCommand
    {
        EntityCommands::Type type;
        EntityHandle handle;
    };

private:
    // Type declarations.
    typedef std::vector<EntitySubscriber*> SubscriberList;
    typedef std::vector<HandleEntry>       HandleList;
    typedef std::vector<EntityCommand>     CommandList;

public:
    EntitySystem();
    ~EntitySystem();

    // Restores class instance to it's original state.
    void Cleanup();

    // Registers an entity subscriber.
    bool RegisterSubscriber(EntitySubscriber* subscriber);

    // Creates an entity.
    EntityHandle CreateEntity();

    // Destroys an entity.
    void DestroyEntity(const EntityHandle& entity);

    // Destroys all entities.
    void DestroyAllEntities();

    // Checks if an entity handle is valid.
    bool IsHandleValid(const EntityHandle& entity) const;

    // Process entity commands.
    void ProcessCommands();

private:
    // Called on creating an entity.
    void OnCreateEntity(const EntityHandle& entity);

    // Called on destroying an entity.
    void OnDestroyEntity(const EntityHandle& entity);

private:
    // List of subscribers.
    SubscriberList m_subscribers;

    // List of commands.
    CommandList m_commands;

    // List of entities.
    HandleList m_handles;

    // List of free handles.
    int  m_freeListDequeue;
    int  m_freeListEnqueue;
    bool m_freeListEmpty;
};
