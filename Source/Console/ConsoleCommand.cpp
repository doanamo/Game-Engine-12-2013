#include "Precompiled.hpp"
#include "ConsoleCommand.hpp"

#include "MainGlobal.hpp"
#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleHistory.hpp"

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
