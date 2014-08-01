#include "Precompiled.hpp"
#include "InputSystem.hpp"
#include "InputComponent.hpp"

#include "Game/Component/ComponentSystem.hpp"

InputSystem::InputSystem() :
    m_initialized(false),
    m_keyboardState(nullptr),
    m_keyboardStateSize(0)
{
}

InputSystem::~InputSystem()
{
    Cleanup();
}

void InputSystem::Cleanup()
{
    m_initialized = false;

    // Keyboard state.
    delete[] m_keyboardState;
    m_keyboardState = nullptr;
    m_keyboardStateSize = 0;
}

bool InputSystem::Initialize(ComponentSystem* componentSystem)
{
    Cleanup();

    // Validate arguments.
    if(componentSystem == nullptr)
        return false;

    // Declare required components.
    componentSystem->Declare<InputComponent>();

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

    // Success!
    return m_initialized = true;
}

void InputSystem::Process(const SDL_Event& event)
{
    assert(m_initialized);

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

void InputSystem::Reset()
{
    assert(m_initialized);

    // Reset keyboard state.
    for(int i = 0; i < m_keyboardStateSize; ++i)
    {
        m_keyboardState[i] = 0;
    }
}

bool InputSystem::IsKeyDown(SDL_Scancode key) const
{
    assert(m_initialized);

    return m_keyboardState[key] == 1;
}

bool InputSystem::IsKeyUp(SDL_Scancode key) const
{
    assert(m_initialized);

    return m_keyboardState[key] == 0;
}
