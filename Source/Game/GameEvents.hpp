#pragma once

#include "Precompiled.hpp"

#include "Game/Entity/EntityHandle.hpp"
#include "Game/Health/HealthComponent.hpp"

//
// Game Events
//

struct HealthChangeEvent
{
    HealthChangeEvent() :
        entity(),
        component(nullptr),
        value(0)
    {
    }

    EntityHandle entity;
    HealthComponent* component;
    int value;
};
