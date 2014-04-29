#pragma once

#include "Precompiled.hpp"
#include "Receiver.hpp"

// Forward declarations.
template<typename Type>
class Receiver;

template<typename Type>
class ReceiverSignature;

//
// Dispatcher
//

template<typename Type>
class Dispatcher
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

private:
    Receiver<Type>* Previous(Receiver<Type>* receiver);

private:
    Receiver<Type>* m_begin;
    Receiver<Type>* m_end;
};

#include "Dispatcher.inl"
