#pragma once

#include "Precompiled.hpp"

//
// Delegate
//  - Implementation based on: http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
//

template<typename ReturnType, typename... Arguments>
class Delegate
{
private:
    typedef void* InstancePtr;
    typedef ReturnType (*FunctionPtr)(InstancePtr, Arguments...);

    template<ReturnType (*Function)(Arguments...)>
    static ReturnType FunctionStub(InstancePtr instance, Arguments... arguments)
    {
        return (Function)(std::forward<Arguments>(arguments)...);
    }

    template<class InstanceType, ReturnType (InstanceType::*Function)(Arguments...)>
    static ReturnType MethodStub(InstancePtr instance, Arguments... arguments)
    {
        return (static_cast<InstanceType*>(instance)->*Function)(std::forward<Arguments>(arguments)...);
    }

public:
    Delegate() :
        m_instance(nullptr),
        m_function(nullptr)
    {
    }

    template<ReturnType (*Function)(Arguments...)>
    void Bind()
    {
        m_instance = nullptr;
        m_function = &FunctionStub<Function>;
    }

    template<class InstanceType, ReturnType (InstanceType::*Function)(Arguments...)>
    void Bind(InstanceType* instance)
    {
        m_instance = instance;
        m_function = &MethodStub<InstanceType, Function>;
    }

    ReturnType Invoke(Arguments... arguments)
    {
        if(m_function == nullptr)
            return;

        return m_function(m_instance, std::forward<Arguments>(arguments)...);
    }

private:
    InstancePtr m_instance;
    FunctionPtr m_function;
};
