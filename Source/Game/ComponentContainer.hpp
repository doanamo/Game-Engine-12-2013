#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"
#include "EntityHandle.hpp"
#include "EntitySubscriber.hpp"

//
// Component Container
//

template<typename Type>
class ComponentContainer : public EntitySubscriber
{
public:
    // Check template type.
    static_assert(std::is_base_of<Component, Type>::value, "Not a component type.");

    // Type declarations.
    typedef std::unordered_map<EntityHandle, Type> ComponentList;
    typedef typename ComponentList::iterator       ComponentIterator;

public:
    ComponentContainer()
    {
    }

    ~ComponentContainer()
    {
    }

    void Cleanup()
    {
        // Free component memory.
        ClearContainer(m_components);
    }

    Type* Create(EntityHandle handle)
    {
        // Create a new component for this entity handle.
        auto result = m_components.emplace(std::make_pair(handle, Type()));

        // Check if the entity already has this component.
        if(result.second == false)
            return nullptr;
    
        // Return a pointer to a newly created component.
        return &result.first->second;
    }

    Type* Lookup(EntityHandle handle)
    {
        // Find a component.
        auto result = m_components.find(handle);
    
        if(result == m_components.end())
            return nullptr;

        // Make sure handles match.
        assert(result->first == handle);

        // Return a pointer to the component.
        return &result->second;
    }

    void Remove(EntityHandle handle)
    {
        // Remove a component.
        m_components.erase(handle);
    }

    void Clear()
    {
        // Remove all components.
        m_components.clear();
    }

    ComponentIterator Begin()
    {
        return m_components.begin();
    }

    ComponentIterator End()
    {
        return m_components.end();
    }

private:
    // Remove component of an entity that's being destroyed.
    void OnDestroyEntity(const EntityHandle& entity)
    {
        Remove(entity);
    }

private:
    // List of components.
    ComponentList m_components;
};
