#include "Precompiled.hpp"
#include "InputState.hpp"

InputState::InputState() :
    m_keyboardState(nullptr),
    m_keyboardStateSize(0)
{
}

InputState::~InputState()
{
    Cleanup();
}

bool InputState::Initialize()
{
    Cleanup();

    // Get keyboard state array size.
    SDL_GetKeyboardState(&m_keyboardStateSize);

    // Create a keyboard state array.
    m_keyboardState = new uint8_t[m_keyboardStateSize];

    if(m_keyboardState == nullptr)
        return false;

    for(int i = 0; i < m_keyboardStateSize; ++i)
    {
        m_keyboardState[i] = 0;
    }

    return true;
}

void InputState::Cleanup()
{
    delete[] m_keyboardState;
    m_keyboardState = nullptr;

    m_keyboardStateSize = 0;
}

void InputState::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        {
            assert(event.key.keysym.scancode <= m_keyboardStateSize);

            if(event.key.keysym.scancode <= m_keyboardStateSize)
            {
                m_keyboardState[event.key.keysym.scancode] = 1;
            }
        }
        break;

    case SDL_KEYUP:
        {
            assert(event.key.keysym.scancode <= m_keyboardStateSize);

            if(event.key.keysym.scancode <= m_keyboardStateSize)
            {
                m_keyboardState[event.key.keysym.scancode] = 0;
            }
        }
        break;
    }
}

void InputState::Reset()
{
    // Reset keyboard state.
    for(int i = 0; i < m_keyboardStateSize; ++i)
    {
        m_keyboardState[i] = 0;
    }
}

bool InputState::IsKeyDown(SDL_Scancode key) const
{
    return m_keyboardState[key] == 1;
}

bool InputState::IsKeyUp(SDL_Scancode key) const
{
    return m_keyboardState[key] == 0;
}
