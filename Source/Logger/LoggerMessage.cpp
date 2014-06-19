#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

#include "MainContext.hpp"

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

LoggerMessage& LoggerMessage::Source(const char* source, unsigned int line)
{
    if(source == nullptr)
        return *this;

    // Set message source.
    m_source = source;
    m_line = line;

    // Replace source path separators.
    std::replace(m_source.begin(), m_source.end(), '\\', '/');

    // Remove base path to source directory.
    m_source.erase(m_source.find(Main::SourceDir()), Main::SourceDir().size());

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
