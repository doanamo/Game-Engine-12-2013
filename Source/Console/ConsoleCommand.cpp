#include "Precompiled.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleHistory.hpp"

#include "MainContext.hpp"

ConsoleCommand::ConsoleCommand(std::string name, FunctionPointer function, std::string description) :
    ConsoleDefinition(name, description), m_function(function)
{
}

ConsoleCommand::~ConsoleCommand()
{
}

void ConsoleCommand::Execute(std::string arguments)
{
    assert(m_function != nullptr);

    m_function(arguments);
}
