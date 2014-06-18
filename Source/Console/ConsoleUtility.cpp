#include "Precompiled.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleHistory.hpp"

#include "MainContext.hpp"

//
// Command Functions
//

void CommandHelp(std::string arguments)
{
    assert(Main::ConsoleSystem().IsValid());

    // Get some more help here.
    if(arguments.empty())
    {
        Log() << "Type \"help <definition>\" for more info on a particular command or variable.";
        return;
    }

    // Locate the first white space.
    std::size_t separator = arguments.find(' ');

    // Get the first argument.
    std::string name(arguments);

    if(separator != std::string::npos)
    {
        name.erase(separator);
    }

    // Get the definition.
    ConsoleDefinition* definition = Main::ConsoleSystem().FindDefinition(name);

    if(definition == nullptr)
    {
        Log() << "Couldn't find \"" << name << "\" definition.";
        return;
    }

    // Print the description.
    Log() << definition->GetDescription();
}

void CommandEcho(std::string arguments)
{
    Log() << arguments;
}

void CommandClear(std::string arguments)
{
    assert(Main::ConsoleHistory().IsValid());

    Main::ConsoleHistory().ClearOutput();
}

void CommandQuit(std::string arguments)
{
    Main::Quit();
}

//
// Console Commands
//

namespace Console
{
    ConsoleCommand help("help", &CommandHelp, "Prints the description of a command or variable.");
    ConsoleCommand echo("echo", &CommandEcho, "Prints provided arguments in the console.");
    ConsoleCommand clear("clear", &CommandClear, "Clears the console output.");
    ConsoleCommand quit("quit", &CommandQuit, "Quits the application.");
}
