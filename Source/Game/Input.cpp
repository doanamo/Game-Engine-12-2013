#include "Precompiled.hpp"
#include "Input.hpp"

Input::Input() :
    m_state(nullptr)
{
}

Input::~Input()
{
}

void Input::SetStateReference(InputState* state)
{
    m_state = state;
}

const InputState* Input::GetState() const
{
    return m_state;
}
