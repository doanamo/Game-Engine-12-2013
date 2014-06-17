#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

//
// Console Variable
//

class ConsoleVariable : public ConsoleDefinition
{
public:
    ConsoleVariable(std::string name, std::string value, std::string description = "");
    ConsoleVariable(std::string name, bool        value, std::string description = "");
    ConsoleVariable(std::string name, int         value, std::string description = "");
    ConsoleVariable(std::string name, float       value, std::string description = "");
    ~ConsoleVariable();

    void Execute(std::string arguments);

    // Assign operators.
    ConsoleVariable& operator=(const std::string& right)
    {
        m_value = right;
        return *this;
    }

    ConsoleVariable& operator=(const char* right)
    {
        m_value = right;
        return *this;
    }

    ConsoleVariable& operator=(const bool& right)
    {
        this->SetBool(right);
        return *this;
    }

    ConsoleVariable& operator=(const int& right)
    {
        this->SetInteger(right);
        return *this;
    }

    ConsoleVariable& operator=(const float& right)
    {
        this->SetFloat(right);
        return *this;
    }

    // Conversion operators.
    operator std::string() const
    {
        return this->GetString();
    }

    operator const char*() const
    {
        return m_value.c_str();
    }

    operator bool() const
    {
        return this->GetBool();
    }

    operator int() const
    {
        return this->GetInteger();
    }

    operator float() const
    {
        return this->GetFloat();
    }

    // Setter methods.
    void SetString(std::string value)
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

    // Getter methods.
    std::string GetString() const
    {
        return m_value;
    }

    bool GetBool() const
    {
        return m_value == "true" || this->GetFloat() != 0.0;
    }

    int GetInteger() const
    {
        int value;

        try
        {
            value = std::stoi(m_value);
        }
        catch(const std::exception&)
        {
            value = 0;
        }

        return value;
    }

    float GetFloat() const
    {
        float value;

        try
        {
            value = std::stof(m_value);
        }
        catch(const std::exception&)
        {
            value = 0.0f;
        }

        return value;
    }

private:
    std::string m_value;
};
