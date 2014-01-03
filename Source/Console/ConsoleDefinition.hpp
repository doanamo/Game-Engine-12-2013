#pragma once

#include "Precompiled.hpp"

class ConsoleSystem;

//
// Console Definition
//

class ConsoleDefinition : public NonCopyable
{
protected:
    ConsoleDefinition(std::string name, std::string description);

public:
    virtual ~ConsoleDefinition();

    virtual void Execute(std::string arguments) = 0;

    std::string GetName() const
    {
        return m_name;
    }

    std::string GetDescription() const
    {
        return m_description;
    }

    static void RegisterStatic();

private:
    std::string m_name;
    std::string m_description;

private:
    // List of static definitions that will be registered 
    // after the console system is initialized.
    ConsoleDefinition*          m_staticNext;
    static ConsoleDefinition*   m_staticHead;
    static bool                 m_staticDone;
};
