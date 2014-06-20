#pragma once

#include "Precompiled.hpp"

//
// Logger Message
//  An object that contains the message and optionaly it's source.
//

class LoggerMessage : public NonCopyable, public std::ostream
{
public:
    LoggerMessage();
    LoggerMessage(LoggerMessage&& other);
    virtual ~LoggerMessage();

    LoggerMessage& Source(const char* source, unsigned int line);
    LoggerMessage& Text(const char* text);

    bool IsEmpty() const
    {
        return m_buffer.str().empty();
    }

    std::string GetText() const
    {
        return m_buffer.str();
    }

    std::string GetSource() const
    {
        return m_source;
    }

    unsigned int GetLine() const
    {
        return m_line;
    }

private:
    std::stringbuf m_buffer;
    std::string    m_source;
    unsigned int   m_line;
};
