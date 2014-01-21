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

    void SetStateReference(InputState* state);
    const InputState* GetState() const;

private:
    InputState* m_state;
};
