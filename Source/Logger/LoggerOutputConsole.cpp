#include "Precompiled.hpp"
#include "LoggerOutputConsole.hpp"
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

    Context::ConsoleHistory().Write(message.GetText().c_str());
}
