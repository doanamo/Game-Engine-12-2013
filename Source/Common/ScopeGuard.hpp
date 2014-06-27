#pragma once

#include "Common/NonCopyable.hpp"

//
// Scope Guard
//  Executes a function when the scope guard goes out of it's scope, at it's destruction.
//
//  Creating a scope guard:
//      int* array = new int[10];
//      auto Cleanup = MakeScopeGuard([&]()
//      {
//          delete[] array;
//      });
//
//  Using a cope guard macro:
//      int* array = new int[10];
//      SCOPE_GUARD(delete[] array);
//

// Scope guard class.
template<typename Type>
class ScopeGuard : public NonCopyable
{
public:
    ScopeGuard(Type function) :
        m_function(function),
        m_enabled(true)
    {
    }

    ~ScopeGuard()
    {
        if(m_enabled)
        {
            m_function();
        }
    }

    void Enable()
    {
        m_enabled = true;
    }

    void Disable()
    {
        m_enabled = false;
    }

    bool IsEnabled() const
    {
        return m_enabled;
    }

private:
    Type m_function;
    bool m_enabled;
};

// Creates a scope guard.
template<typename Type>
ScopeGuard<Type> MakeScopeGuard(Type function)
{
    return ScopeGuard<Type>(function);
}

// Utility macros.
#define SCOPE_GUARD(code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_MAKE(code)
#define SCOPE_GUARD_MAKE(code) MakeScopeGuard([&]() { code; })
#define SCOPE_GUARD_NAME(line) SCOPE_GUARD_STRING(line)
#define SCOPE_GUARD_STRING(line) scopeGuard_Line ## line
