#pragma once

#include "Common/NonCopyable.hpp"
#include "LoggerOutput.hpp"
#include "LoggerMessage.hpp"
#include "LoggerScopedMessage.hpp"

#include <fstream>

// Log macro.
#ifdef _DEBUG
	#define Log() LoggerScopedMessage(Context::logger).Source(__FILE__, __LINE__)
#else
	#define Log() LoggerScopedMessage(Context::logger)
#endif

// Forward declarations.
class LoggerMessage;

//
// Logger
//

class Logger : public NonCopyable
{
public:
	Logger();
	~Logger();

	void AddOutput(LoggerOutput* output);
	void RemoveOutput(LoggerOutput* output);

	void Write(const LoggerMessage& message);

private:
	std::vector<LoggerOutput*> m_outputs;
};
