#pragma once

#include "Precompiled.hpp"

//
// Console Definition
//  - Base class used by console variable/command classes.
//

class ConsoleDefinition : public NonCopyable
{
protected:
    ConsoleDefinition(std::string name, std::string description);

public:
    virtual ~ConsoleDefinition();

    // Called when a definition has been executed in the console.
    virtual void Execute(std::string arguments) = 0;

    // Accessor methods.
    std::string GetName() const
    {
        return m_name;
    }

    std::string GetDescription() const
    {
        return m_description;
    }

    bool HasExecuted() const
    {
        return m_executed;
    }

    bool HasChanged() const
    {
        return m_changed;
    }

protected:
    // Sets intermediate states.
    void Executed();
    void Changed();

    // Resets intermediate states.
    void ResetIntermediateState();

private:
    std::string m_name;
    std::string m_description;

    bool m_executed;
    bool m_changed;

public:
    // Use this string as a name to mark definition as internal.
    static const char* Internal;

    // Call at the very beginning of main() to finalize
    // static instances of this class, so static and normal
    // instanced can be distinguished.
    static void FinalizeStatic();

private:
    // Allow console system to register static definitions.
    friend class ConsoleSystem;

    // List of static definitions that were created
    // before the console system has been initialized.
    ConsoleDefinition*          m_staticNext;
    static ConsoleDefinition*   m_staticHead;
    static bool                 m_staticDone;
};
