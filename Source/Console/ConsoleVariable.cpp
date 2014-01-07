#include "Precompiled.hpp"
#include "ConsoleVariable.hpp"

ConsoleVariable::ConsoleVariable(std::string name, std::string value, std::string description) :
    ConsoleDefinition(name, description), m_value(value)
{
}

ConsoleVariable::ConsoleVariable(std::string name, const char* value, std::string description) :
    ConsoleDefinition(name, description), m_value(value)
{
}

ConsoleVariable::ConsoleVariable(std::string name, bool value, std::string description) :
    ConsoleDefinition(name, description)
{
    this->SetBool(value);
}

ConsoleVariable::ConsoleVariable(std::string name, int value, std::string description) :
    ConsoleDefinition(name, description)
{
    this->SetInteger(value);
}

ConsoleVariable::ConsoleVariable(std::string name, float value, std::string description) :
    ConsoleDefinition(name, description)
{
    this->SetFloat(value);
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
