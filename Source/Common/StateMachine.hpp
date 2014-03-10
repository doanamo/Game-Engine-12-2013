#pragma once

#include "NonCopyable.hpp"

//
// State
//

template<typename Owner = void*>
class State
{
public:
    virtual void OnEnter(Owner owner)
    {
        OnEnter();
    }

    virtual void OnExit(Owner owner)
    {
        OnExit();
    }

    virtual void OnEnter()
    {
    }

    virtual void OnExit()
    {
    }
};

//
// State Machine
//

template<typename State = State<void*>, typename Owner = void*>
class StateMachine
{
public:
    StateMachine(Owner owner = nullptr) :
        m_owner(owner),
        m_current(nullptr),
        m_previous(nullptr)
    {
    }

    ~StateMachine()
    {
    }

    void Cleanup()
    {
        m_current = nullptr;
        m_previous = nullptr;
    }

    void ChangeState(State state)
    {
        if(state == m_current)
            return;

        // Exit the current state.
        m_previous = m_current;

        if(m_current != nullptr)
        {
            m_current->OnExit(m_owner);
        }

        // Enter the new state.
        m_current = state;

        if(m_current != nullptr)
        {
            m_current->OnEnter(m_owner);
        }
    }

    void RevertState()
    {
        this->ChangeState(m_previous);
    }

    void SetCurrentState(State state)
    {
        m_current = state;
    }

    void SetPreviousState(State state)
    {
        m_previous = state;
    }

    State GetState() const
    {
        return m_current;
    }

    State GetCurrentState() const
    {
        return m_current;
    }

    State GetPreviousState() const
    {
        return m_previous;
    }

    bool IsValid() const
    {
        return m_current != nullptr;
    }

private:
    Owner m_owner;
    State m_current;
    State m_previous;
};
