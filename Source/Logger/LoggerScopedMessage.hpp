#pragma once

#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

// Forward declarations.
class Logger;

//
// Logger Scoped Message
//  Log message wrapper that sends itself to the 
//  logger after it reaches the end of it's scope.
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
