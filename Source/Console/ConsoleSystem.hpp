#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class ConsoleDefinition;

//
// Console System
//  - Resposible for managing console definitions and executing commands on them.
//

class ConsoleSystem : public NonCopyable
{
public:
    ConsoleSystem();
    ~ConsoleSystem();

    bool Initialize();
    void Cleanup();

    void Execute(std::string input);

    void RegisterDefinition(ConsoleDefinition* definition);
    void UnregisterDefinition(ConsoleDefinition* definition);

    ConsoleDefinition* FindDefinition(std::string name);

    bool IsValid() const
    {
        return m_initialized;
    }

private:
    // List of registered definitions.
    std::map<std::string, ConsoleDefinition*> m_definitions;

    bool m_initialized;
};
