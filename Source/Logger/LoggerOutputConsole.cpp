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

    Main::ConsoleHistory().WriteOutput(message.GetText().c_str());
}
