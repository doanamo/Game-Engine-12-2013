#pragma once

#include "Common/NonCopyable.hpp"
#include "LoggerMessage.hpp"
#include "LoggerScopedMessage.hpp"

#include <fstream>

// Log macro.
#define Log() LoggerScopedMessage(Context::logger).Source(__FILE__, __LINE__)

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

	bool Open(std::string filename);
	void Close();

	void Write(const LoggerMessage& message);

private:
	std::ofstream m_file;
};
