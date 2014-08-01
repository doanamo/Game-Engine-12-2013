#pragma once

#include "Precompiled.hpp"
#include "InputState.hpp"

#include "Game/Component/Component.hpp"

// Forward declarations.
class Entity;

//
// Input Component
//

class InputComponent : public Component
{
public:
    InputComponent();
    ~InputComponent();

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
