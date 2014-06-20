#pragma once

#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

//
// Logger Output
//  - Base interface for logger outputs.
//

class LoggerOutput
{
public:
    virtual void Write(const LoggerMessage& message) = 0;
};
