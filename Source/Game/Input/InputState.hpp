#pragma once

#include "Precompiled.hpp"

//
// Input State
//

class InputState
{
public:
    InputState();
    ~InputState();

    bool Initialize();
    void Cleanup();

    void Process(const SDL_Event& event);
    void Reset();

    bool IsKeyDown(SDL_Scancode key) const;
    bool IsKeyUp(SDL_Scancode key) const;

private:
    // Keyboard state.
    uint8_t* m_keyboardState;
    int m_keyboardStateSize;
};
