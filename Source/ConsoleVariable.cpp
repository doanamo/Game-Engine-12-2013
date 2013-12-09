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
		std::cout << GetName() << " = " << GetString() << std::endl;
	}
	else
	{
		m_value = arguments;
	}
}
