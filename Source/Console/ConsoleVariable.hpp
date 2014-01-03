#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

//
// Console Variable
//

// TODO: Handle invalid string to numeric conversions (for now it just crashes).

class ConsoleVariable : public ConsoleDefinition
{
public:
    ConsoleVariable(std::string name, std::string value, std::string description);
    ~ConsoleVariable();

    void Execute(std::string arguments);

    void SetString(const char* value)
    {
        m_value = value;
    }

    void SetBool(bool value)
    {
        m_value = value ? "true" : "false";
    }

    void SetInteger(int value)
    {
        m_value = std::to_string(value);
    }

    void SetFloat(float value)
    {
        m_value = std::to_string(value);
    }

    std::string GetString() const
    {
        return m_value;
    }

    bool GetBool() const
    {
        return m_value == "true";
    }

    int GetInteger() const
    {
        return std::stoi(m_value);
    }

    float GetFloat() const
    {
        return std::stof(m_value);
    }

private:
    std::string m_value;
};
