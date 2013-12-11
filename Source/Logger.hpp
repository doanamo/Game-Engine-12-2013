#pragma once

#include "Common/NonCopyable.hpp"

#include <sstream>
#include <fstream>

//
// Logger
//

// Log macro.
#define Log() LoggerScopedMessage(Context::logger).Source(__FILE__, __LINE__)

// Forward declarations.
class LoggerMessage;

// Logger class.
class Logger : public NonCopyable
{
public:
	Logger(std::string filename);
	~Logger();

	void Write(const LoggerMessage& message);

private:
	std::ofstream m_file;
};

// Logger message class.
class LoggerMessage : public NonCopyable, public std::ostream
{
public:
	LoggerMessage();
	LoggerMessage(LoggerMessage&& other);
	virtual ~LoggerMessage();

	LoggerMessage& Source(const char* filename, unsigned int line);
	LoggerMessage& Text(const char* text);

	bool IsEmpty() const
	{
		return m_buffer.str().empty();
	}

	std::string GetText() const
	{
		return m_buffer.str();
	}

	const char* GetFilename() const
	{
		return m_filename;
	}

	unsigned int GetLine() const
	{
		return m_line;
	}

private:
	std::stringbuf m_buffer;
	const char*    m_filename;
	unsigned int   m_line;
};

// Logger scoped message class.
class LoggerScopedMessage : public LoggerMessage
{
public:
	LoggerScopedMessage(Logger* logger);
	LoggerScopedMessage(LoggerScopedMessage&& other);
	~LoggerScopedMessage();

private:
	Logger* m_logger;
};
