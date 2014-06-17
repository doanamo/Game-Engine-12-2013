#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

//
// Console Command
//

class ConsoleCommand : public ConsoleDefinition
{
public:
    typedef void (*FunctionPointer)(std::string);

public:
    ConsoleCommand(std::string name, FunctionPointer function, std::string description = "");
    ~ConsoleCommand();

    void Execute(std::string arguments);

    // Call operator.
    ConsoleCommand& operator()(std::string arguments = "")
    {
        this->Execute(arguments);
        return *this;
    }

private:
    FunctionPointer m_function;
};
