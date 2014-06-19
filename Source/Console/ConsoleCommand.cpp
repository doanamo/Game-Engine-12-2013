#include "Precompiled.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleHistory.hpp"

#include "MainContext.hpp"

ConsoleCommand::ConsoleCommand(std::string name, FunctionPointer function, std::string description) :
    ConsoleDefinition(name, description), m_function(function)
{
    // Function pointer can be null.
}

ConsoleCommand::~ConsoleCommand()
{
}

void ConsoleCommand::Execute(std::string arguments)
{
    // Call command function.
    if(m_function != nullptr)
    {
        m_function(arguments);
    }

    // Set executed state.
    this->Executed();
}
