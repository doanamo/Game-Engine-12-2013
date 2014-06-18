#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class ConsoleSystem;

//
// Console Definition
//

class ConsoleDefinition : public NonCopyable
{
public:
    // Use this string as a name to mark definition as internal.
    static const char* Internal;

protected:
    ConsoleDefinition(std::string name, std::string description = "");

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

private:
    std::string m_name;
    std::string m_description;

public:
    // Call at the very beginning of main() to finalize
    // static instances of this class, so static and normal
    // instanced can be distinguished.
    static void FinalizeStatic();

private:
    friend class ConsoleSystem;

    // List of static definitions that were created
    // before the console system has been initialized.
    ConsoleDefinition*          m_staticNext;
    static ConsoleDefinition*   m_staticHead;
    static bool                 m_staticDone;
};
