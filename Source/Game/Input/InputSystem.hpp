#pragma once

#include "Precompiled.hpp"
#include "InputState.hpp"

// Forward declarations.
class Services;

//
// Input System
//

class InputSystem : public InputState
{
public:
    InputSystem();
    ~InputSystem();

    bool Initialize(const Services& services);
    void Cleanup();

    void Process(const SDL_Event& event);
    void Reset();

    bool IsKeyDown(int key) const;
    bool IsKeyUp(int key) const;

private:
    // System state.
    bool m_initialized;

    // Keyboard state.
    uint8_t* m_keyboardState;
    int m_keyboardStateSize;
};
