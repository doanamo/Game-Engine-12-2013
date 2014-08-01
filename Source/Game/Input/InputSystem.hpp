#pragma once

#include "Precompiled.hpp"
#include "InputState.hpp"

// Forward declarations.
class ComponentSystem;

//
// Input System
//

class InputSystem : public InputState
{
public:
    InputSystem();
    ~InputSystem();

    bool Initialize(ComponentSystem* componentSystem);
    void Cleanup();

    void Process(const SDL_Event& event);
    void Reset();

    bool IsKeyDown(SDL_Scancode key) const;
    bool IsKeyUp(SDL_Scancode key) const;

private:
    // System state.
    bool m_initialized;

    // Keyboard state.
    uint8_t* m_keyboardState;
    int m_keyboardStateSize;
};
