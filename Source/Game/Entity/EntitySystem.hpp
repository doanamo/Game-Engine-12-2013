#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

// Forward declarations.
class Services;
class EventSystem;

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
            // Entity handle has been allocated but is not being used.
            None = 0,

            // Entity handle has been created and is valid.
            Valid =  1 << 0,

            // Entity handle has been created and is active.
            Active = 1 << 1,

            // Entity handle has been scheduled to be destroyed.
            Destroy = 1 << 2,
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
    typedef std::vector<HandleEntry>       HandleList;
    typedef std::vector<EntityCommand>     CommandList;

public:
    EntitySystem();
    ~EntitySystem();

    // Initializes the entity system.
    bool Initialize(const Services& services);

    // Restores class instance to it's original state.
    void Cleanup();

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

    // Returns the number of active entities.
    unsigned int GetEntityCount() const;

private:
    // System state.
    bool m_initialized;

    // Event system.
    EventSystem* m_eventSystem;

    // List of commands.
    CommandList m_commands;

    // List of entities.
    HandleList m_handles;

    // Number of active entities.
    unsigned int m_entityCount;

    // List of free handles.
    int  m_freeListDequeue;
    int  m_freeListEnqueue;
    bool m_freeListEmpty;
};
