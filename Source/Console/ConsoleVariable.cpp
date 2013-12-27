#include "Precompiled.hpp"
#include "ConsoleVariable.hpp"

ConsoleVariable::ConsoleVariable(std::string name, std::string value, std::string description) :
	ConsoleDefinition(name, description), m_value(value)
{
}

ConsoleVariable::~ConsoleVariable()
{
}

void ConsoleVariable::Execute(std::string arguments)
{
	if(arguments.empty())
	{
		Log() << GetName() << " = " << GetString();
	}
	else
	{
		m_value = arguments;
	}
}
