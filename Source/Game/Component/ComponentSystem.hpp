#pragma once

#include "Precompiled.hpp"

#include "Game/Component/ComponentPool.hpp"
#include "Game/Entity/EntitySystem.hpp"

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
        m_entitySystem(nullptr)
    {
    }

    ~ComponentSystem()
    {
        Cleanup();
    }

    void Cleanup()
    {
        m_entitySystem = nullptr;
        ClearContainer(m_pools);
    }

    bool Initialize(EntitySystem* entitySystem)
    {
        Cleanup();

        // Validate arguments.
        if(entitySystem == nullptr)
            return false;

        m_entitySystem = entitySystem;

        // Success!
        return true;
    }

    template<typename Type>
    void CreatePool()
    {
        assert(m_entitySystem != nullptr);

        // Validate component type.
        static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

        // Create a component pool instance.
        auto pool = std::make_unique<ComponentPool<Type>>();

        // Register the pool as entity system subscriber.
        m_entitySystem->RegisterSubscriber(pool.get());

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
        pool.Remove(handle);
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
    EntitySystem* m_entitySystem;
    ComponentPoolList m_pools;
};
