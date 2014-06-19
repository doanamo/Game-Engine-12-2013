#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

//
// Console Command
//  - Definition that calls a function when executed in console.
//

class ConsoleCommand : public ConsoleDefinition
{
public:
    // Type of function that can be used.
    typedef void (*FunctionPointer)(std::string);

public:
    ConsoleCommand(std::string name, FunctionPointer function, std::string description = "");
    ~ConsoleCommand();

    // Execute command.
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
