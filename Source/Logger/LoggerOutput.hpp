#pragma once

#include "Precompiled.hpp"
#include "LoggerMessage.hpp"

//
// Logger Output
//

class LoggerOutput
{
public:
	virtual void Write(const LoggerMessage& message) = 0;
};
