#pragma once

#include "Common/NonCopyable.hpp"
#include "LoggerOutput.hpp"
#include "LoggerMessage.hpp"
#include "LoggerScopedMessage.hpp"

#include <fstream>

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
    std::vector<LoggerOutput*> m_outputs;

    // Global logger instance.
    static Logger* m_global;
};
