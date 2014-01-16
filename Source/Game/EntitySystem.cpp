#include "Precompiled.hpp"
#include "EntitySystem.hpp"
#include "EntityManager.hpp"
#include "ComponentSystem.hpp"

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
    // Remove all entities.
    DestroyAllEntities();

    ClearContainer(m_entities);
    ClearContainer(m_handles);

    // Reset the free list queue.
    m_freeListDequeue = InvalidQueueElement;
    m_freeListEnqueue = InvalidQueueElement;
    m_freeListEmpty = true;

    // Remove all systems.
    for(auto it = m_systems.begin(); it != m_systems.end(); ++it)
    {
        (*it)->m_entitySystem = nullptr;
    }

    ClearContainer(m_systems);

    // Remove all managers.
    for(auto it = m_managers.begin(); it != m_managers.end(); ++it)
    {
        (*it)->m_entitySystem = nullptr;
    }

    ClearContainer(m_managers);
}

bool EntitySystem::RegisterManager(EntityManager* manager)
{
    // Check if pointer is null.
    if(manager == nullptr)
        return false;

    // Check if the manager is already in use.
    if(manager->m_entitySystem != nullptr)
        return false;

    // Check if we already have the manager added.
    auto it = std::find(m_managers.begin(), m_managers.end(), manager);

    if(it != m_managers.end())
        return false;

    // Add manager to the list.
    manager->m_entitySystem = this;
    m_managers.push_back(manager);

    return true;
}

bool EntitySystem::RegisterSystem(ComponentSystem* system)
{
    // Check if pointer is null.
    if(system == nullptr)
        return false;

    // Check if the system is already in use.
    if(system->m_entitySystem != nullptr)
        return false;

    // Check if we already have the system added.
    auto it = std::find(m_systems.begin(), m_systems.end(), system);

    if(it != m_systems.end())
        return false;

    // Add system to the list.
    system->m_entitySystem = this;
    m_systems.push_back(system);

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

    // Inform about the created entity.
    OnCreateEntity(&entity);

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

    // Inform about the soon to be destroyed entity.
    OnDestroyEntity(&entity);

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

void EntitySystem::DestroyAllEntities()
{
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

    // Check if the handle versions match.
    if(handleEntry.handle.version != handle.version)
        return false;

    return true;
}

void EntitySystem::OnCreateEntity(Entity* entity)
{
    // Inform managers about entity insertion.
    for(auto it = m_managers.begin(); it != m_managers.end(); ++it)
    {
        (*it)->OnCreateEntity(entity);
    }
}

void EntitySystem::OnDestroyEntity(Entity* entity)
{
    // Inform managers about entity removal.
    for(auto it = m_managers.begin(); it != m_managers.end(); ++it)
    {
        (*it)->OnDestroyEntity(entity);
    }
}

void EntitySystem::Update(float TimeDelta)
{
    // Set the current time delta.
    m_timeDelta = TimeDelta;

    // Process entities with each system.
    for(auto system = m_systems.begin(); system != m_systems.end(); ++system)
    {
        (*system)->PrepareProcessing();

        for(auto entity = m_entities.begin(); entity != m_entities.end(); ++entity)
        {
            (*system)->Process(&(*entity));
        }

        (*system)->FinishProcessing();
    }
}

float EntitySystem::GetTimeDelta()
{
    return m_timeDelta;
}
