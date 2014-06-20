#pragma once

#include "Precompiled.hpp"
#include "LoggerOutput.hpp"

//
// Logger Output Console
//  Writes log messages to console history output.
//

class LoggerOutputConsole : public LoggerOutput
{
public:
    LoggerOutputConsole();
    ~LoggerOutputConsole();

    void Cleanup();

    void Write(const LoggerMessage& message);
};
