#pragma once

#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

// Forward declarations.
class Logger;

//
// Logger Scoped Message
//

class LoggerScopedMessage : public LoggerMessage
{
public:
    LoggerScopedMessage(Logger* logger);
    LoggerScopedMessage(LoggerScopedMessage&& other);
    ~LoggerScopedMessage();

private:
    Logger* m_logger;
};
