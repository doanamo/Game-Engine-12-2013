#pragma once

#include "Precompiled.hpp"
#include "Common/Dispatcher.hpp"
#include "Common/Receiver.hpp"

//
// Event System
//

class EventSystem
{
    // Type declarations.
    typedef std::unique_ptr<DispatcherInterface> DispatcherPtr;
    typedef std::unordered_map<std::type_index, DispatcherPtr> DispatcherList;
    typedef DispatcherList::value_type DispatcherPair;

public:
    EventSystem()
    {
    }

    ~EventSystem()
    {
        Cleanup();
    }

    void Cleanup()
    {
        ClearContainer(m_dispatchers);
    }

    bool Initialize()
    {
        Cleanup();

        return true;
    }

    template<typename Type>
    void Subscribe(Receiver<Type>& receiver)
    {
        // Check if we already have this event type declared.
        auto it = m_dispatchers.find(typeid(Type));

        if(it == m_dispatchers.end())
        {
            // Create a dispatcher for this event type.
            auto dispatcher = std::make_unique<Dispatcher<Type>>();

            // Add dispatcher to the list.
            auto pair = DispatcherPair(typeid(Type), std::move(dispatcher));
            auto result = m_dispatchers.insert(std::move(pair));

            assert(result.second == true);
            assert(result.first != m_dispatchers.end());

            // Set iterator to the newly inserted element.
            it = result.first;
        }

        // Cast the pointer that we already know is a dispatcher.
        Dispatcher<Type>* dispatcher = reinterpret_cast<Dispatcher<Type>*>(it->second.get());

        // Subscribe receiver to the dispatcher.
        dispatcher->Subscribe(receiver);
    }

    template<typename Type>
    void Unsubscribe(Receiver<Type>& receiver)
    {
        // Warning: This method won't be used. Generally, when Receiver instances are
        // destroyed, they are unsubscribing directly from a Dispatcher. Event system
        // can't know when this happens and so a declared event type along with it's
        // empty dispatcher remain in the registry. If this ever becomes a problem,
        // wrap the Receiver class (EventReceiver?).

        // Find the dispatcher for this event type.
        auto it = m_dispatchers.find(typeid(Type));

        if(it == m_dispatchers.end())
            return;

        // Cast the pointer that we already know is a dispatcher.
        Dispatcher<Type>* dispatcher = reinterpret_cast<Dispatcher<Type>*>(it->second.get());

        // Unsubscribe receiver from the dispatcher.
        dispatcher->Unsubscribe(receiver);
    }

    template<typename Type>
    void Dispatch(const Type& event)
    {
        // Find the dispatcher for this event type.
        auto it = m_dispatchers.find(typeid(Type));

        if(it == m_dispatchers.end())
            return;

        // Cast the pointer that we already know is a dispatcher.
        Dispatcher<Type>* dispatcher = reinterpret_cast<Dispatcher<Type>*>(it->second.get());

        // Dispatch event to all receivers.
        dispatcher->Dispatch(event);
    }

private:
    DispatcherList m_dispatchers;
};
