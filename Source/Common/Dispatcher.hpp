#pragma once

#include "Precompiled.hpp"
#include "Receiver.hpp"

// Forward declarations.
template<typename Type>
class Receiver;

template<typename Type>
class ReceiverSignature;

//
// Dispatcher Interface
//

class DispatcherInterface
{
protected:
    DispatcherInterface()
    {
    }

public:
    ~DispatcherInterface()
    {
    }
};

//
// Dispatcher
//  Holds a list of subscribed receivers that can be invoked all at once. 
//  More safe than using raw delegates, as unsubscribing is automated at 
//  receiver's destruction. No dangling pointers are left.
//  A single dispatcher instance can have multiple receivers subscribed.
//
//  Example usage:
//      struct EventData { /* ... */ };
//      
//      void Class::FunctionA(const EventData& event) { /*...*/ }
//      void Class::FunctionB(const EventData& event) { /*...*/ }
//      Class instance;
//      
//      Receiver<EventData> receiverA;
//      receiverA.Bind<Class, &Class::FunctionA>(&instance);
//      
//      Receiver<EventData> receiverB;
//      receiverB.Bind<Class, &Class::FunctionB>(&instance);
//      
//      Dispatcher<EventData> dispatcher;
//      dispatcher.Subscribe(receiverA);
//      dispatcher.Subscribe(ReceiverSignature(receiverB));
//      dispatcher.Dispatch(EventData(/* ... */));
//

template<typename Type>
class Dispatcher : public DispatcherInterface
{
public:
    Dispatcher();
    ~Dispatcher();

    void Cleanup();

    void Subscribe(const ReceiverSignature<Type>& signature);
    void Subscribe(Receiver<Type>* receiver);

    void Unsubscribe(const ReceiverSignature<Type>& signature);
    void Unsubscribe(Receiver<Type>* receiver);

    void Dispatch(const Type& event);

    bool HasSubscribers() const;

private:
    Receiver<Type>* Previous(Receiver<Type>* receiver);

private:
    Receiver<Type>* m_begin;
    Receiver<Type>* m_end;
};

#include "Dispatcher.inl"
