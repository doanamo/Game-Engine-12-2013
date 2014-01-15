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
        if(m_value == "true")
        {
            return true;
        }
        else
        if(m_value == "false")
        {
            return false;
        }
        else
        if(GetFloat() != 0.0)
        {
            return true;
        }

        return false;
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
