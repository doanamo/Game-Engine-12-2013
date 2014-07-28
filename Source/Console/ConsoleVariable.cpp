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
    this->ResetIntermediateState();
}

ConsoleVariable::ConsoleVariable(std::string name, int value, std::string description) :
    ConsoleDefinition(name, description)
{
    this->SetInteger(value);
    this->ResetIntermediateState();
}

ConsoleVariable::ConsoleVariable(std::string name, float value, std::string description) :
    ConsoleDefinition(name, description)
{
    this->SetFloat(value);
    this->ResetIntermediateState();
}

ConsoleVariable::~ConsoleVariable()
{
}

void ConsoleVariable::Execute(std::string arguments)
{
    // Print or assign new value.
    if(arguments.empty())
    {
        // Print current value.
        Log() << this->GetName() << " = " << this->GetString();
    }
    else
    {
        // Assign new value.
        m_value = arguments;

        // Set changed state.
        this->Changed();
    }

    // Set executed state.
    this->Executed();
}

int ConsoleVariable::GetInteger() const
{
    return std::strtol(m_value.c_str(), nullptr, 10);
}

float ConsoleVariable::GetFloat() const
{
    return std::strtof(m_value.c_str(), nullptr);
}
