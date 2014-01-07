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
    ConsoleVariable(std::string name, const char* value, std::string description = "");
    ConsoleVariable(std::string name, bool        value, std::string description = "");
    ConsoleVariable(std::string name, int         value, std::string description = "");
    ConsoleVariable(std::string name, float       value, std::string description = "");

    ~ConsoleVariable();

    void Execute(std::string arguments);

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

    ConsoleVariable& operator=(bool right)
    {
        this->SetBool(right);
        return *this;
    }

    ConsoleVariable& operator=(int right)
    {
        this->SetInteger(right);
        return *this;
    }

    ConsoleVariable& operator=(float right)
    {
        this->SetFloat(right);
        return *this;
    }

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
