#include "Precompiled.hpp"
#include "EntitySystem.hpp"
#include "EntitySubsystem.hpp"

namespace
{
    // Constant variables.
    const int MaximumIdentifier   = std::numeric_limits<int>::max();
    const int InvalidIdentifier   = 0;
    const int InvalidEntityIndex  = -1;
    const int InvalidNextFree     = -1;
    const int InvalidQueueElement = -1;
}

EntitySystem::EntitySystem() :
    m_validEntityIndex(InvalidEntityIndex),
    m_freeListDequeue(InvalidQueueElement),
    m_freeListEnqueue(InvalidQueueElement),
    m_freeListEmpty(true),
    m_timeDelta(0.0f)
{
}

EntitySystem::~EntitySystem()
{
    Cleanup();
}

void EntitySystem::Cleanup()
{
    // Process remaining commands.
    ProcessCommands();

    ClearContainer(m_commands);

    // Remove all entities.
    DestroyAllEntities();

    ClearContainer(m_entities);
    ClearContainer(m_handles);

    // Reset valid entity index range.
    m_validEntityIndex = InvalidEntityIndex;

    // Reset the free list queue.
    m_freeListDequeue = InvalidQueueElement;
    m_freeListEnqueue = InvalidQueueElement;
    m_freeListEmpty = true;

    // Remove all subsystems.
    for(auto it = m_subsystems.begin(); it != m_subsystems.end(); ++it)
    {
        (*it)->m_entitySystem = nullptr;
    }

    ClearContainer(m_subsystems);
}

bool EntitySystem::RegisterSubsystem(EntitySubsystem* subsystem)
{
    // Check if pointer is null.
    if(subsystem == nullptr)
        return false;

    // Check if the subsystem is already in use.
    if(subsystem->m_entitySystem != nullptr)
        return false;

    // Check if we already have the subsystem added.
    auto it = std::find(m_subsystems.begin(), m_subsystems.end(), subsystem);

    if(it != m_subsystems.end())
        return false;

    // Add subsystem to the list.
    subsystem->m_entitySystem = this;
    m_subsystems.push_back(subsystem);

    return true;
}

EntitySystem::EntityResult EntitySystem::CreateEntity()
{
    // Check if we reached the numerical limits.
    assert(m_handles.size() != MaximumIdentifier);

    // Create a new handle if the free list queue is empty.
    if(m_freeListEmpty)
    {
        // Create an entity handle.
        EntityHandle handle;
        handle.identifier = m_handles.size() + 1;
        handle.version = 0;

        // Create a handle entry.
        HandleEntry entry;
        entry.handle = handle;
        entry.entityIndex = InvalidEntityIndex;
        entry.nextFree = InvalidNextFree;

        m_handles.push_back(entry);

        // Add new handle entry to the free list queue.
        int handleIndex = m_handles.size() - 1;

        m_freeListDequeue = handleIndex;
        m_freeListEnqueue = handleIndex;
        m_freeListEmpty = false;
    }

    // Retrieve an unused handle from the free list.
    HandleEntry& handleEntry = m_handles[m_freeListDequeue];

    // Update the free list queue.
    if(m_freeListDequeue == m_freeListEnqueue)
    {
        // If there was only one element in the queue.
        // Set the free list queue state to empty.
        m_freeListDequeue = InvalidQueueElement;
        m_freeListEnqueue = InvalidQueueElement;
        m_freeListEmpty = true;
    }
    else
    {
        // If there were more than a single element in the queue.
        // Set the beginning of the queue to the next free element.
        m_freeListDequeue = handleEntry.nextFree;
    }

    handleEntry.nextFree = InvalidNextFree;

    // Create an entity.
    m_entities.emplace_back();

    int entityIndex = m_entities.size() - 1;
    Entity& entity = m_entities[entityIndex];

    // Set the entity data.
    entity.m_entitySystem = this;
    entity.m_handle = handleEntry.handle;

    // Update the handle entry to point to the newly created entity.
    handleEntry.entityIndex = entityIndex;

    // Add a create entity commands.
    EntityCommand command;
    command.type = EntityCommands::Create;
    command.handleEntry = handleEntry;
    
    m_commands.push_back(command);

    // Return the handle.
    return EntityResult(&entity);
}

Entity* EntitySystem::LookupEntity(const EntityHandle& handle)
{
    // Check if the handle is valid.
    if(!IsHandleValid(handle))
        return nullptr;

    // Locate the handle entry.
    int handleIndex = handle.identifier - 1;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if entity index is in a valid range.
    if(handleEntry.entityIndex > InvalidEntityIndex)
        return nullptr;

    // Locate the entity.
    Entity& entity = m_entities[handleEntry.entityIndex];

    // Make sure the entity has the corrent handle description.
    assert(entity.GetHandle().identifier == handle.identifier);
    assert(entity.GetHandle().version == handle.version);

    return &entity;
}

void EntitySystem::DestroyEntity(const EntityHandle& handle)
{
    // Check if the handle is valid.
    if(!IsHandleValid(handle))
        return;

    // Locate the handle entry.
    int handleIndex = handle.identifier - 1;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if the entity belongs to this system.
    Entity& entity = m_entities[handleEntry.entityIndex];

    if(entity.m_entitySystem != this)
        return;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handleEntry = handleEntry;

    m_commands.push_back(command);
}

void EntitySystem::DestroyAllEntities()
{
    // Process entity commands.
    ProcessCommands();

    // Don't do anything if there are no entities.
    if(m_entities.empty())
        return;

    // Inform about entities soon to be destroyed.
    for(auto it = m_entities.begin(); it != m_entities.end(); ++it)
    {
        OnDestroyEntity(&(*it));
    }

    // Destroy all entities.
    m_entities.clear();

    // Reset the range of valid entities.
    m_validEntityIndex = InvalidEntityIndex;

    // Invalidate all handles.
    for(unsigned int i = 0; i < m_handles.size(); ++i)
    {
        HandleEntry& handleEntry = m_handles[i];

        // Increment the handle version to invalidate it.
        handleEntry.handle.version += 1;

        // Set invalid entity index.
        handleEntry.entityIndex = InvalidEntityIndex;

        // Create a chain for the free list queue.
        handleEntry.nextFree = i + 1;
    }

    // Close the free list queue chain at the end.
    int lastHandleIndex = m_handles.size() - 1;
    m_handles[lastHandleIndex].nextFree = InvalidNextFree;

    // Set the free list chain.
    m_freeListDequeue = 0;
    m_freeListEnqueue = lastHandleIndex;
    m_freeListEmpty = false;
}

bool EntitySystem::IsHandleValid(const EntityHandle& handle) const
{
    // Check if the handle identifier is valid.
    if(handle.identifier <= InvalidIdentifier)
        return false;

    if(handle.identifier > (int)m_handles.size())
        return false;

    // Locate the handle entry.
    int handleIndex = handle.identifier - 1;
    const HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if the handle is used.
    if(handleEntry.entityIndex == InvalidEntityIndex)
        return false;

    // Check if the entity index is valid.
    if(handleEntry.entityIndex > m_validEntityIndex)
        return false;

    // Check if the handle versions match.
    if(handleEntry.handle.version != handle.version)
        return false;

    return true;
}

void EntitySystem::OnCreateEntity(Entity* entity)
{
    // Inform subsystems about entity creation.
    for(auto it = m_subsystems.begin(); it != m_subsystems.end(); ++it)
    {
        (*it)->OnCreateEntity(entity);
    }
}

void EntitySystem::OnDestroyEntity(Entity* entity)
{
    // Inform managers about entity destroyal.
    for(auto it = m_subsystems.begin(); it != m_subsystems.end(); ++it)
    {
        (*it)->OnDestroyEntity(entity);
    }
}

void EntitySystem::ProcessCommands()
{
    // Process entity commands.
    for(auto command = m_commands.begin(); command != m_commands.end(); ++command)
    {
        switch(command->type)
        {
        case EntityCommands::Create:
            {
                // Increase the valid entity range.
                m_validEntityIndex += 1;

                // The index of this entity should match.
                assert(m_validEntityIndex == command->handleEntry.entityIndex);
                
                // Inform about the created entity.
                OnCreateEntity(&m_entities[command->handleEntry.entityIndex]);
            }
            break;

        case EntityCommands::Destroy:
            {
                // Locate the current handle entry.
                int handleIndex = command->handleEntry.handle.identifier - 1;
                HandleEntry& handleEntry = m_handles[handleIndex];

                // Check if the command handle entry is still valid.
                assert(handleEntry.handle.identifier == command->handleEntry.handle.identifier);

                if(handleEntry.handle.version != command->handleEntry.handle.version)
                {
                    // Entity was propably destroyed twice in a single frame.
                    continue;
                }

                // Decrease the valid entity range.
                m_validEntityIndex -= 1;

                // Inform about the soon to be destroyed entity.
                OnDestroyEntity(&m_entities[handleEntry.entityIndex]);

                // Reallocate the entity that handle entry points at.
                if(handleEntry.entityIndex != m_entities.size() - 1)
                {
                    // Move the last entity in the place of the one that will be removed 
                    // to preserve contiguous memory of valid entities without holes.
                    m_entities[handleEntry.entityIndex] = std::move(m_entities[m_entities.size() - 1]);

                    // Update the handle entry of the moved entity.
                    Entity& movedEntity = m_entities[handleEntry.entityIndex];

                    int movedHandleIndex = movedEntity.m_handle.identifier - 1;
                    HandleEntry& movedHandleEntry = m_handles[movedHandleIndex];
                    movedHandleEntry.entityIndex = handleEntry.entityIndex;
                }

                m_entities.pop_back();

                // Set the invalid entity index.
                handleEntry.entityIndex = InvalidEntityIndex;

                // Increment the handle version to invalidate it.
                handleEntry.handle.version += 1;

                // Add the handle entry to the free list queue.
                if(m_freeListEmpty)
                {
                    // If there are no elements in the queue.
                    // Set the element as the only one in the queue.
                    m_freeListDequeue = handleIndex;
                    m_freeListEnqueue = handleIndex;
                    m_freeListEmpty = false;
                }
                else
                {
                    assert(m_handles[m_freeListEnqueue].nextFree == InvalidNextFree);

                    // If there are already other elements in the queue.
                    // Add the element to the end of the queue chain.
                    m_handles[m_freeListEnqueue].nextFree = handleIndex;
                    m_freeListEnqueue = handleIndex;
                }
            }
            break;
        }
    }

    // Clear processed entity commands.
    m_commands.clear();
}

void EntitySystem::Update(float timeDelta)
{
    // Process entity commands.
    ProcessCommands();

    // Set the current time delta.
    m_timeDelta = timeDelta;

    // Process entities with each system.
    for(auto subsystem = m_subsystems.begin(); subsystem != m_subsystems.end(); ++subsystem)
    {
        (*subsystem)->PrepareProcessing();

        for(int entityIndex = 0; entityIndex < (int)m_entities.size(); ++entityIndex)
        {
            // Check if entity is in a valid range (it could have not been processed yet after creating).
            if(entityIndex > m_validEntityIndex)
                break;

            // Process an entity.
            (*subsystem)->Process(&m_entities[entityIndex]);
        }

        (*subsystem)->FinishProcessing();
    }
}

float EntitySystem::GetTimeDelta()
{
    return m_timeDelta;
}
