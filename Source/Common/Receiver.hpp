#pragma once

#include "Precompiled.hpp"
#include "Dispatcher.hpp"
#include "Delegate.hpp"

// Forward delcarations.
template<typename Type>
class Dispatcher;

template<typename Type>
class Receiver;

//
// Receiver Signature
//

template<typename Type>
class ReceiverSignature
{
public:
    friend Dispatcher<Type>;

public:
    ReceiverSignature(Receiver<Type>& receiver) :
        m_receiver(&receiver)
    {
    }

private:
    Receiver<Type>* m_receiver;
};

//
// Receiver
//

template<typename Type>
class Receiver : public Delegate<void, const Type&>
{
public:
    friend Dispatcher<Type>;

public:
    Receiver() :
        m_subject(nullptr),
        m_next(nullptr)
    {
    }

    virtual ~Receiver()
    {
        Cleanup();
    }

    void Cleanup()
    {
        // Unregister from the dispatcher.
        if(m_subject)
        {
            m_subject->Unsubscribe(this);
        }
    }

private:
    void Receive(const Type& event)
    {
        // Call the bound delegate function.
        Invoke(event);
    }

private:
    Dispatcher<Type>* m_subject;
    Receiver<Type>* m_next;
};
