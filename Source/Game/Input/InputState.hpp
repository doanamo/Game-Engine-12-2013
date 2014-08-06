#pragma once

#include "Precompiled.hpp"

//
// Input State
//

class InputState
{
public:
    virtual bool IsKeyDown(int key) const = 0;
    virtual bool IsKeyUp(int key) const = 0;
};
