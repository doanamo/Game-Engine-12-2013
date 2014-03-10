#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"

//
// Base Frame
//

class BaseFrame : public State<>
{
public:
    virtual bool Process(const SDL_Event& event)
    {
        return false;
    }

    virtual void Update(float dt)
    {
    }

    virtual void Draw()
    {
    }
};
