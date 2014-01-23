#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

// Forward declarations.
class Entity;
class InputState;

//
// Input Component
//

class Input : public Component
{
public:
    Input();
    ~Input();

    void SetStateReference(InputState* state)
    {
        m_state = state;
    }

    const InputState* GetState() const
    {
        return m_state;
    }

private:
    InputState* m_state;
};
