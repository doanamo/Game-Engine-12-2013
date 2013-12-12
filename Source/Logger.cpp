#include "Precompiled.hpp"
#include "Logger.hpp"

#include <ctime>
#include <iomanip>

Logger::Logger()
{
}

Logger::~Logger()
{
	Close();
}

bool Logger::Open(std::string filename)
{
	Close();

	// Open a file.
	m_file.open(filename);

	if(!m_file.is_open())
		return false;

	// Write session start.
	time_t timeData = time(nullptr);
	tm* timeInfo = localtime(&timeData);

	m_file << "Session started at ";
	m_file << std::setfill('0');
	m_file << std::setw(4) << timeInfo->tm_year + 1900 << "-";
	m_file << std::setw(2) << timeInfo->tm_mon + 1     << "-";
	m_file << std::setw(2) << timeInfo->tm_mday        << " ";
	m_file << std::setw(2) << timeInfo->tm_hour        << ":";
	m_file << std::setw(2) << timeInfo->tm_min         << ":";
	m_file << std::setw(2) << timeInfo->tm_sec;
	m_file << "\n\n";

	m_file.flush();

	return true;
}

void Logger::Close()
{
	if(m_file.is_open())
	{
		// Write session end.
		time_t timeData = time(nullptr);
		tm* timeInfo = localtime(&timeData);

		m_file << "\n";
		m_file << "Session ended at ";
		m_file << std::setfill('0');
		m_file << std::setw(4) << timeInfo->tm_year + 1900 << "-";
		m_file << std::setw(2) << timeInfo->tm_mon + 1     << "-";
		m_file << std::setw(2) << timeInfo->tm_mday        << " ";
		m_file << std::setw(2) << timeInfo->tm_hour        << ":";
		m_file << std::setw(2) << timeInfo->tm_min         << ":";
		m_file << std::setw(2) << timeInfo->tm_sec;

		m_file.flush();

		// Close a file.
		m_file.close();
	}
}

void Logger::Write(const LoggerMessage& message)
{
	if(!m_file.is_open())
		return;

	if(message.IsEmpty())
		return;

	// Message prefix.
	time_t timeData = time(nullptr);
	tm* timeInfo = localtime(&timeData);

	m_file << "[";
	m_file << std::setfill('0');
	m_file << std::setw(2) << timeInfo->tm_hour << ":";
	m_file << std::setw(2) << timeInfo->tm_min  << ":";
	m_file << std::setw(2) << timeInfo->tm_sec;
	m_file << "] ";

	// Message text.
	m_file << message.GetText();

	// Message source.
	if(message.GetFilename() != nullptr && message.GetLine() != 0)
	{
		m_file << " (";
		m_file << message.GetFilename();
		m_file << ":";
		m_file << message.GetLine();
		m_file << ")";
	}

	// Message suffix.
	m_file << "\n";

	// Flush the output.
	m_file.flush();
}

LoggerMessage::LoggerMessage() :
	std::ostream(&m_buffer),
	m_filename(nullptr),
	m_line(0)
{
}

LoggerMessage::LoggerMessage(LoggerMessage&& other) :
	std::ostream(&m_buffer)
{
	m_buffer = std::move(other.m_buffer);

	m_filename = other.m_filename;
	other.m_filename = nullptr;

	m_line = other.m_line;
	other.m_line = 0;
}

LoggerMessage::~LoggerMessage()
{
}

LoggerMessage& LoggerMessage::Source(const char* filename, unsigned int line)
{
	m_filename = filename;
	m_line = line;

	return *this;
}

LoggerMessage& LoggerMessage::Text(const char* text)
{
	m_buffer.str("");

	if(text != nullptr)
	{
		m_buffer.sputn(text, strlen(text));
	}

	return *this;
}

LoggerScopedMessage::LoggerScopedMessage(Logger* logger) :
	m_logger(logger)
{
}

LoggerScopedMessage::LoggerScopedMessage(LoggerScopedMessage&& other) :
	LoggerMessage(std::forward<LoggerScopedMessage>(other))
{
	m_logger = other.m_logger;
	other.m_logger = nullptr;
}

LoggerScopedMessage::~LoggerScopedMessage()
{
	if(m_logger != nullptr)
	{
		m_logger->Write(*this);
	}
}
