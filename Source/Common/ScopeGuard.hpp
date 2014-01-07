#pragma once

#include "NonCopyable.hpp"

//
// Scope Guard
//

// Scope guard class.
template<typename Type>
class ScopeGuard : public NonCopyable
{
public:
    ScopeGuard(Type function) :
        m_function(function)
    {
    }

    ~ScopeGuard()
    {
        m_function();
    }

private:
    Type m_function;
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
