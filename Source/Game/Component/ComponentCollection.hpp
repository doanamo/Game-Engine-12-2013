#pragma once

#include "Precompiled.hpp"

#include "Game/Component/ComponentPool.hpp"
#include "Game/Entity/EntityHandle.hpp"

//
// Component Collection
//

class ComponentCollection
{
public:
    // Type declarations.
    typedef std::unordered_map<std::type_index, void*> ComponentPoolList;

public:
    ComponentCollection()
    {
    }

    ~ComponentCollection()
    {
        Cleanup();
    }

    void Cleanup()
    {
        ClearContainer(m_pools);
    }

    template<typename Type>
    void Register(ComponentPool<Type>* pool)
    {
        assert(pool != nullptr);

        // Add pool to the collection.
        auto pair = std::pair<std::type_index, void*>(typeid(Type), pool);
        auto result = m_pools.insert(pair);
        assert(result.second == true);
    }

    template<typename Type>
    Type* Create(EntityHandle handle)
    {
        // Get the component pool.
        ComponentPool<Type>* pool = GetComponentPool<Type>();

        if(pool == nullptr)
            return nullptr;

        // Create and return the component.
        return pool->Create();
    }

    template<typename Type>
    Type* Lookup(EntityHandle handle)
    {
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
        // Find pool by component type.
        auto it = m_pools.find(typeid(Type));

        if(it == m_pools.end())
            return nullptr;

        // Cast the pointer that we already know is a component pool.
        ComponentPool<Type>* pool = reinterpret_cast<ComponentPool<Type>*>(it->second);

        // Return the pool.
        return pool;
    }

private:
    ComponentPoolList m_pools;
};
