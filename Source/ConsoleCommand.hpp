#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

//
// Console Command
//

class ConsoleCommand : public ConsoleDefinition
{
public:
	typedef void (*FunctionPointer)(std::string);

public:
	ConsoleCommand(std::string name, FunctionPointer function, std::string description);

	~ConsoleCommand();

	void Execute(std::string arguments);

private:
	FunctionPointer m_function;
};
