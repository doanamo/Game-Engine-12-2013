#include "Precompiled.hpp"
#include "LoggerOutputConsole.hpp"

#include "MainContext.hpp"
#include "Console/ConsoleHistory.hpp"

LoggerOutputConsole::LoggerOutputConsole()
{
}

LoggerOutputConsole::~LoggerOutputConsole()
{
    Cleanup();
}

void LoggerOutputConsole::Cleanup()
{
}

void LoggerOutputConsole::Write(const LoggerMessage& message)
{
    if(message.IsEmpty())
        return;

    // Message text.
    std::string output = message.GetText();

    // Message source.
    #ifdef _DEBUG
        if(!message.GetSource().empty() && message.GetLine() != 0)
        {
            output += " (";
            output += message.GetSource();
            output += ":";
            output += std::to_string(message.GetLine());
            output += ")";
        }
    #endif

    // Write to console history.
    Main::ConsoleHistory().WriteOutput(output.c_str());
}
