#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

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
