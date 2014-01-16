#pragma once

#include "Precompiled.hpp"
#include "Entity.hpp"

// Forward declarations.
class ComponentSystem;
class EntityManager;

//
// Entity System
//

class EntitySystem
{
private:
    // Handle entry structure.
    struct HandleEntry
    {
        EntityHandle handle;
        int entityIndex;
        int nextFree;
    };

    // Entity result structure.
    struct EntityResult
    {
        // Constructor.
        EntityResult(Entity* entity) :
            entity(entity)
        {
            assert(entity != nullptr);
        }

        // Conversion operators.
        operator Entity*() const
        {
            return entity;
        }

        operator EntityHandle() const
        {
            return entity->GetHandle();
        }

        // Result data.
        Entity* entity;
    };

private:
    // Type declarations.
    typedef std::vector<ComponentSystem*> SystemList;
    typedef std::vector<EntityManager*>   ManagerList;
    typedef std::vector<Entity>           EntityList;
    typedef std::vector<HandleEntry>      HandleList;

public:
    EntitySystem();
    ~EntitySystem();

    // Restores class instance to it's original state.
    void Cleanup();

    // Registers an entity manager.
    bool RegisterManager(EntityManager* manager);

    // Registers a component system.
    bool RegisterSystem(ComponentSystem* system);

    // Creates an entity.
    EntityResult CreateEntity();

    // Lookups an entity.
    Entity* LookupEntity(const EntityHandle& handle);

    // Destroys an entity.
    void DestroyEntity(const EntityHandle& handle);

    // Destroys all entities.
    void DestroyAllEntities();

    // Checks if an entity handle is valid.
    bool IsHandleValid(const EntityHandle& handle) const;

    // Updates all entities.
    void Update(float timeDelta);

    // Gets the current time delta.
    float GetTimeDelta();

private:
    // Called on creating an entity.
    void OnCreateEntity(Entity* entity);

    // Called on destroying an entity.
    void OnDestroyEntity(Entity* entity);

private:
    // List of systems and managers.
    SystemList m_systems;
    ManagerList m_managers;

    // List of entities.
    EntityList m_entities;
    HandleList m_handles;

    // List of free handles.
    int  m_freeListDequeue;
    int  m_freeListEnqueue;
    bool m_freeListEmpty;

    // Current frame time delta.
    float m_timeDelta;
};
