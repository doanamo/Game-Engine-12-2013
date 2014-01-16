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
    bool operator==(const EntityHandle& other)
    {
        return identifier == other.identifier && version == other.version;
    }

    // Handle data.
    int identifier;
    int version;
};
