#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"
#include "CollisionSystem.hpp"

//
// Script
//

class Script
{
public:
    friend class ScriptSystem;

public:
    Script() :
        m_touched(false)
    {
    }

    virtual void OnCreate(EntityHandle self)
    {
    }

    virtual void OnUpdate(EntityHandle self, float timeDelta)
    {
    }

    virtual void OnCollision(CollisionObject& self, CollisionObject& other)
    {
    }

private:
    bool m_touched;
};
