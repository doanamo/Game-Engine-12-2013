#pragma once

#include "Precompiled.hpp"

#include "Common/NonCopyable.hpp"
#include "Logger/LoggerOutput.hpp"
#include "Logger/LoggerMessage.hpp"
#include "Logger/LoggerScopedMessage.hpp"

// Log macro.
#ifdef _DEBUG
    #define Log() LoggerScopedMessage(Logger::GetGlobal()).Source(__FILE__, __LINE__)
#else
    #define Log() LoggerScopedMessage(Logger::GetGlobal())
#endif

// Forward declarations.
class LoggerMessage;

//
// Logger
//  - Entry point for log messages that will be redirected to registered outputs.
//

class Logger : public NonCopyable
{
public:
    Logger();
    ~Logger();

    void Cleanup();

    void AddOutput(LoggerOutput* output);
    void RemoveOutput(LoggerOutput* output);

    void Write(const LoggerMessage& message);

public:
    static void SetGlobal(Logger* logger);
    static Logger* GetGlobal();

private:
    // List of logger outputs.
    std::vector<LoggerOutput*> m_outputs;

    // Global logger instance.
    static Logger* m_global;
};
