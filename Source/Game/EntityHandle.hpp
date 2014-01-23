#pragma once

#include "Precompiled.hpp"

//
// Entity Handle
//

struct EntityHandle
{
    // Constructor.
    EntityHandle() :
        identifier(0),
        version(0)
    {
    }

    // Comparison operator.
    bool operator==(const EntityHandle& other) const
    {
        return identifier == other.identifier && version == other.version;
    }

    bool operator!=(const EntityHandle& other) const
    {
        return identifier != other.identifier || version != other.version;
    }

    // Handle data.
    int identifier;
    int version;
};

//
// Handle Hash
//

namespace std
{
    template<>
    struct hash<EntityHandle>
    {
        std::size_t operator()(const EntityHandle& handle) const
        {
            // Use the identifier as a hash.
            return handle.identifier;
        }
    };
}
