#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Script
//

class Script
{
public:
    virtual void OnUpdate(EntityHandle entity, float timeDelta)
    {
        assert(true);
    }
};
