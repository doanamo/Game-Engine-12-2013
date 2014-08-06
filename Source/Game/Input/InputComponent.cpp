#include "Precompiled.hpp"
#include "InputComponent.hpp"
#include "InputState.hpp"

InputComponent::InputComponent() :
    m_state(nullptr)
{
}

InputComponent::~InputComponent()
{
}

bool InputComponent::IsKeyDown(int key) const
{
    if(!m_state)
        return false;

    return m_state->IsKeyDown(key);
}

bool InputComponent::IsKeyUp(int key) const
{
    if(!m_state)
        return true;

    return m_state->IsKeyUp(key);
}
