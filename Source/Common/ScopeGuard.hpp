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
//  Using a scope guard macro:
//      int* array = new int[10];
//      SCOPE_GUARD(delete[] array);
//
//  Using a conditional scope guard macro:
//      bool cleanup = true;
//      int* array = new int[10];
//      SCOPE_GUARD_IF(cleanup, delete[] array);
//
//  Using braced scope guard macros:
//      int* array = new int[10];
//      SCOPE_GUARD_BEGIN()
//          delete[] array;
//          array = nullptr;
//      SCOPE_GUARD_END()
//
//  Using braced conditonal scope guard macros:
//      bool cleanup = true;
//      int* array = new int[10];
//      SCOPE_GUARD_BEGIN(cleanup)
//          delete[] array;
//          array = nullptr;
//      SCOPE_GUARD_END()
//

// Scope guard class.
template<typename Type>
class ScopeGuard : private NonCopyable
{
public:
    ScopeGuard(Type function) :
        m_function(function),
        m_enabled(true)
    {
    }

    ScopeGuard(ScopeGuard<Type>&& other)
    {
        m_function = std::move(other.m_function);
        m_enabled = other.m_enabled;
        other.m_enabled = false;
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

// Scope guard condition.
class ScopeGuardCondition
{
public:
    ScopeGuardCondition()
    {
        m_condition = true;
    }

    ScopeGuardCondition(bool condition)
    {
        m_condition = condition;
    }

    explicit operator bool()
    {
        return m_condition;
    }

private:
    bool m_condition;
};

// Utility macros.
#define SCOPE_GUARD_BEGIN(...) auto SCOPE_GUARD_NAME(__LINE__) = MakeScopeGuard([&]() { if(ScopeGuardCondition(__VA_ARGS__)) { 
#define SCOPE_GUARD_END() } });

#define SCOPE_GUARD(code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_MAKE(code)
#define SCOPE_GUARD_MAKE(code) MakeScopeGuard([&]() { code; })

#define SCOPE_GUARD_IF(condition, code) auto SCOPE_GUARD_NAME(__LINE__) = SCOPE_GUARD_IF_MAKE(condition, code)
#define SCOPE_GUARD_IF_MAKE(condition, code) MakeScopeGuard([&]() { if(condition) { code; } })

#define SCOPE_GUARD_NAME(line) SCOPE_GUARD_STRING(line)
#define SCOPE_GUARD_STRING(line) scopeGuard_Line ## line
