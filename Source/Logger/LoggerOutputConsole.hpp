#pragma once

#include "Precompiled.hpp"
#include "LoggerOutput.hpp"

//
// Logger Output Console
//

class LoggerOutputConsole : public LoggerOutput
{
public:
	LoggerOutputConsole();
	~LoggerOutputConsole();

	void Cleanup();

	void Write(const LoggerMessage& message);
};
