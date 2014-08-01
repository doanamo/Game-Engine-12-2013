#pragma once

#include "Precompiled.hpp"

//
// Services
//  Conveniently holds pointers to instances of different types.
//

class Services
{
public:
    // Type declarations.
    typedef std::unordered_map<std::type_index, void*> InstanceList;
    typedef InstanceList::value_type InstancePair;

public:
    Services()
    {
    }

    ~Services()
    {
        Cleanup();
    }

    void Cleanup()
    {
        ClearContainer(m_instances);
    }

    template<typename Type>
    void Set(Type* instance)
    {
        assert(instance != nullptr);

        // Create an instance entry (if it doesn't exists).
        auto pair = InstancePair(typeid(Type), nullptr);
        auto result = m_instances.insert(pair);
        auto it = result.first;

        // Set the instance.
        it->second = instance;
    }

    template<typename Type>
    Type* Get() const
    {
        // Find instance of this type.
        auto it = m_instances.find(typeid(Type));

        if(it == m_instances.end())
            return nullptr;

        // Cast and return instance pointer.
        return reinterpret_cast<Type*>(it->second);
    }

private:
    InstanceList m_instances;
};
