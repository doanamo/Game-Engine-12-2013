#include "Precompiled.hpp"
#include "LoggerOutputFile.hpp"

#include <ctime>
#include <iomanip>

LoggerOutputFile::LoggerOutputFile(std::string filename)
{
	Open(filename);
}

LoggerOutputFile::~LoggerOutputFile()
{
	Close();
}

bool LoggerOutputFile::Open(std::string filename)
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

void LoggerOutputFile::Close()
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

void LoggerOutputFile::Write(const LoggerMessage& message)
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
