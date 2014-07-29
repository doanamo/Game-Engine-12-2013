#include "Precompiled.hpp"
#include "LoggerOutputFile.hpp"

LoggerOutputFile::LoggerOutputFile()
{
}

LoggerOutputFile::~LoggerOutputFile()
{
    Cleanup();
}

bool LoggerOutputFile::Open(std::string filename)
{
    Cleanup();

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

void LoggerOutputFile::Cleanup()
{
    // Close the file.
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
    #ifdef _DEBUG
        if(!message.GetSource().empty())
        {
            m_file << " (";
            m_file << message.GetSource();

            if(message.GetLine() != 0)
            {
                m_file << ":";
                m_file << message.GetLine();
            }

            m_file << ")";
        }
    #endif

    // Message suffix.
    m_file << "\n";

    // Flush the output.
    m_file.flush();
}
