#pragma once

#include "Precompiled.hpp"

#include "Game/Component/Component.hpp"

// Forward declarations.
class InputState;

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

    const InputState* GetStateReference() const
    {
        return m_state;
    }

    bool IsKeyDown(int key) const;
    bool IsKeyUp(int key) const;

private:
    InputState* m_state;
};
