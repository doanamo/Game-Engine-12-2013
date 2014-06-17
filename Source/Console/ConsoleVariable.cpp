#include "Precompiled.hpp"
#include "ConsoleVariable.hpp"

ConsoleVariable::ConsoleVariable(std::string name, std::string value, std::string description) :
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
        // Print current value.
        Log() << this->GetName() << " = " << this->GetString();
    }
    else
    {
        // Assign new value.
        m_value = arguments;
    }
}
