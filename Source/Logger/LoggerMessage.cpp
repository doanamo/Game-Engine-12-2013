#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

#include "MainGlobal.hpp"

LoggerMessage::LoggerMessage() :
    std::ostream(&m_buffer),
    m_source(),
    m_line(0)
{
}

LoggerMessage::LoggerMessage(LoggerMessage&& other) :
    std::ostream(&m_buffer)
{
    m_buffer = std::move(other.m_buffer);

    m_source = other.m_source;
    other.m_source = nullptr;

    m_line = other.m_line;
    other.m_line = 0;
}

LoggerMessage::~LoggerMessage()
{
}

LoggerMessage& LoggerMessage::SetText(const char* text)
{
    m_buffer.str("");

    if(text != nullptr)
    {
        m_buffer.sputn(text, strlen(text));
    }

    return *this;
}

LoggerMessage& LoggerMessage::SetSource(const char* source)
{
    if(source == nullptr)
        return *this;

    // Set message source.
    m_source = source;

    // Replace source path separators.
    std::replace(m_source.begin(), m_source.end(), '\\', '/');

    // Remove base path to source directory.
    std::size_t position = m_source.find(Main::GetSourceDir());

    if(position != std::string::npos)
    {
        m_source.erase(position, Main::GetSourceDir().size());
    }

    return *this;
}

LoggerMessage& LoggerMessage::SetLine(unsigned int line)
{
    // Set message line.
    m_line = line;

    return *this;
}
