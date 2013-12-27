#include "Precompiled.hpp"
#include "LoggerOutputConsole.hpp"
#include "Console/ConsoleHistory.hpp"

LoggerOutputConsole::LoggerOutputConsole()
{
}

LoggerOutputConsole::~LoggerOutputConsole()
{
}

void LoggerOutputConsole::Write(const LoggerMessage& message)
{
	if(message.IsEmpty())
		return;

	if(Context::consoleHistory != nullptr)
	{
		Context::consoleHistory->Write(message.GetText().c_str());
	}
}
