#pragma once

#include "Precompiled.hpp"

//
// Input State
//

class InputState
{
public:
    virtual bool IsKeyDown(SDL_Scancode key) const = 0;
    virtual bool IsKeyUp(SDL_Scancode key) const = 0;
};
