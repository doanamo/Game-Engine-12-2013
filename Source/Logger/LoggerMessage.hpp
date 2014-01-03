#pragma once

#include "Precompiled.hpp"

#include <sstream>

//
// Logger Message
//

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
