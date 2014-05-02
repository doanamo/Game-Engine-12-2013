#include "Dispatcher.hpp"

template<typename Type>
Dispatcher<Type>::Dispatcher() :
    m_begin(nullptr),
    m_end(nullptr)
{
}

template<typename Type>
Dispatcher<Type>::~Dispatcher()
{
    Cleanup();
}

template<typename Type>
void Dispatcher<Type>::Cleanup()
{
    // Unsubscribe all receivers.
    Receiver<Type>* iterator = m_begin;
    
    while(iterator != nullptr)
    {
        Receiver<Type>* receiver = iterator;

        // Advance to the next receiver.
        iterator = iterator->m_next;

        // Unsubscribe a receiver.
        receiver->m_subject = nullptr;
        receiver->m_next = nullptr;
    }
    
    m_begin = nullptr;
    m_end = nullptr;
}

template<typename Type>
void Dispatcher<Type>::Subscribe(const ReceiverSignature<Type>& signature)
{
    Subscribe(signature.m_receiver);
}

template<typename Type>
void Dispatcher<Type>::Subscribe(Receiver<Type>* receiver)
{
    // Check if receiver is valid.
    if(receiver == nullptr)
        return;

    // Check if receiver is already subscribed somewhere else.
    if(receiver->m_subject != nullptr)
        return;

    assert(receiver->m_next == nullptr);
    
    // Add receiver to the linked list.
    if(m_begin == nullptr)
    {
        assert(m_end == nullptr);
        m_begin = receiver;
        m_end = receiver;
    }
    else
    {
        assert(m_end != nullptr);
        m_end->m_next = receiver;
        m_end = receiver;
    }

    // Set dispatcher as receiver's subject.
    receiver->m_subject = this;
}

template<typename Type>
void Dispatcher<Type>::Unsubscribe(const ReceiverSignature<Type>& signature)
{
    Unsubscribe(signature.m_receiver);
}

template<typename Type>
void Dispatcher<Type>::Unsubscribe(Receiver<Type>* receiver)
{
    // Check if receiver is valid.
    if(receiver == nullptr)
        return;

    // Check if receiver is subscribed to this dispatcher.
    if(receiver->m_subject != this)
        return;

    // Remove receiver from the linked list.
    if(m_begin == receiver)
    {
        if(m_end == receiver)
        {
            m_begin = nullptr;
            m_end = nullptr;
        }
        else
        {
            m_begin = receiver->m_next; 
        }
    }
    else
    {
        Receiver<Type>* previous = Previous(receiver);
        assert(previous != nullptr);

        previous->m_next = receiver->m_next;

        if(m_end == receiver)
        {
            m_end = previous;
        }
    }

    // Remove dispatcher as receiver's subject.
    receiver->m_subject = nullptr;
    receiver->m_next = nullptr;
}

template<typename Type>
void Dispatcher<Type>::Dispatch(const Type& event)
{
    // Send an event to all receivers.
    Receiver<Type>* iterator = m_begin;
        
    while(iterator != nullptr)
    {
        // Send an event to a receiver.
        iterator->Receive(event);

        // Advance to the next receiver.
        iterator = iterator->m_next;
    }
}

template<typename Type>
Receiver<Type>* Dispatcher<Type>::Previous(Receiver<Type>* receiver)
{
    // Iterate through all receivers.
    Receiver<Type>* iterator = m_begin;
    Receiver<Type>* previous = nullptr;

    while(iterator != nullptr)
    {
        // Check if it's the one we are looking for.
        if(iterator == receiver)
            break;

        // Save the previous iterator.
        previous = iterator;

        // Iterate to the next receiver.
        iterator = iterator->m_next;
    }

    // Return the previous receiver.
    return previous;
}
