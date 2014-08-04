#pragma once

#include "Precompiled.hpp"

#include "Common/Services.hpp"
#include "Common/Receiver.hpp"
#include "Game/Component/ComponentPool.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"

//
// Component System
//

class ComponentSystem
{
public:
    // Type declarations.
    typedef std::unique_ptr<ComponentPoolInterface> ComponentPoolPtr;
    typedef std::unordered_map<std::type_index, ComponentPoolPtr> ComponentPoolList;
    typedef ComponentPoolList::value_type ComponentPoolPair;

public:
    ComponentSystem() :
        m_eventSystem(nullptr),
        m_entitySystem(nullptr)
    {
    }

    ~ComponentSystem()
    {
        Cleanup();
    }

    void Cleanup()
    {
        m_eventSystem = nullptr;
        m_entitySystem = nullptr;

        ClearContainer(m_pools);

        m_receiverEntityDestroyed.Cleanup();
    }

    bool Initialize(const Services& services)
    {
        Cleanup();

        // Initialization state.
        bool initialized = false;
        SCOPE_GUARD_IF(!initialized, Cleanup());

        // Get required services.
        m_eventSystem = services.Get<EventSystem>();
        if(m_eventSystem == nullptr) return false;

        m_entitySystem = services.Get<EntitySystem>();
        if(m_entitySystem == nullptr) return false;

        // Bind event receivers.
        m_receiverEntityDestroyed.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroyedEvent>(this);

        // Subscribe event receivers.
        m_eventSystem->Subscribe<GameEvent::EntityDestroyed>(m_receiverEntityDestroyed);

        // Success!
        return initialized = true;
    }

    template<typename Type>
    void Declare()
    {
        assert(m_entitySystem != nullptr);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Check if component type was already declared.
        auto it = m_pools.find(typeid(Type));

        if(it != m_pools.end())
            return;

        // Create a component pool instance.
        auto pool = std::make_unique<ComponentPool<Type>>();

        // Add pool to the collection.
        auto pair = ComponentPoolPair(typeid(Type), std::move(pool));
        auto result = m_pools.insert(std::move(pair));

        assert(result.second == true);
    }

    template<typename Type>
    Type* Create(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Create and return the component.
        return pool->Create(handle);
    }

    template<typename Type>
    Type* Lookup(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Lookup and return the component.
        return pool->Lookup(handle);
    }

    template<typename Type>
    void Remove(EntityHandle handle)
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return;

        // Remove a component.
        pool->Remove(handle);
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator Begin()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return ComponentPool<Type>::ComponentIterator();

        // Return the iterator.
        return pool->Begin();
    }

    template<typename Type>
    typename ComponentPool<Type>::ComponentIterator End()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return ComponentPool<Type>::ComponentIterator();

        // Return the iterator.
        return pool->End();
    }

    template<typename Type>
    ComponentPool<Type>* GetComponentPool()
    {
        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
            return nullptr;

        // Cast the pointer that we already know is a component pool.
        ComponentPool<Type>* pool = reinterpret_cast<ComponentPool<Type>*>(it->second.get());

        // Return the pool.
        return pool;
    }

private:
    void OnEntityDestroyedEvent(const GameEvent::EntityDestroyed& event)
    {
        for(auto& pair : m_pools)
        {
            pair.second->Remove(event.entity);
        }
    }

private:
    // Game systems.
    EventSystem* m_eventSystem;
    EntitySystem* m_entitySystem;

    // Component pools.
    ComponentPoolList m_pools;

    // Event receivers.
    Receiver<GameEvent::EntityDestroyed> m_receiverEntityDestroyed;
};
