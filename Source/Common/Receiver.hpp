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
// Receiver
//  Invokes a function/method after receiving a signal from a dispatcher.
//  A single receiver instance can be subscribed to only one dispatcher.
//  See dispatcher header for more information.
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
        if(m_subject != nullptr)
        {
            m_subject->Unsubscribe(*this);
            assert(m_next == nullptr);
        }
    }

private:
    void Receive(const Type& event)
    {
        // Call the bound delegate function.
        this->Invoke(event);
    }

private:
    Dispatcher<Type>* m_subject;
    Receiver<Type>* m_next;
};

//
// Receiver Signature
//  Object wrapping a receiver that can be safely passed without
//  being worried that it could possibly be modified on the way.
//

template<typename Type>
class ReceiverSignature
{
public:
    friend Dispatcher<Type>;

public:
    ReceiverSignature(Receiver<Type>& receiver) :
        m_receiver(receiver)
    {
    }

private:
    Receiver<Type>& m_receiver;
};
