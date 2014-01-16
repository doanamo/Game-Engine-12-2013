#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "Component.hpp"

// Forward declarations.
class EntitySystem;

//
// Entity
//

class Entity
{
public:
    // Friend declarations.
    friend EntitySystem;

public:
    Entity();
    ~Entity();

    // Move constructor and operator.
    Entity(Entity&& other);
    Entity& operator=(Entity&& other);

    // Adds a component.
    template<typename Type>
    bool InsertComponent(std::unique_ptr<Type>& component);

    // Gets a component.
    template<typename Type>
    Type* GetComponent();

    template<typename Type>
    const Type* GetComponent() const;

    // Gets the entity system that owns this entity .
    EntitySystem*       GetEntitySystem();
    const EntitySystem* GetEntitySystem() const;

    // Gets the entity handle.
    const EntityHandle& GetHandle() const;

private:
    // Entity system.
    EntitySystem* m_entitySystem;

    // Entity handle.
    EntityHandle m_handle;

    // Entity components.
    typedef std::unique_ptr<Component> ComponentPtr;
    std::vector<ComponentPtr> m_components;
};

//
// Template Methods
//

template<typename Type>
bool Entity::InsertComponent(std::unique_ptr<Type>& component)
{
    // Check if the pointer is null.
    if(component == nullptr)
        return false;

    // Check if the entity already has a component of the same type.
    for(auto it = m_components.begin(); it != m_components.end(); ++it)
    {
        if(typeid(**it) == typeid(*component))
            return false;
    }

    // Add the component.
    m_components.push_back(std::move(component));

    return true;
}

template<typename Type>
Type* Entity::GetComponent()
{
    // Call the const function to avoid code duplication.
    return const_cast<Type*>(static_cast<const Entity*>(this)->GetComponent<Type>());
}

template<typename Type>
const Type* Entity::GetComponent() const
{
    // Find the component.
    for(auto it = m_components.begin(); it != m_components.end(); ++it)
    {
        if(typeid(**it) == typeid(Type))
            return static_cast<Type*>((*it).get());
    }

    return nullptr;
}
