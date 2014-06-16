#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class ConsoleDefinition;
class ConsoleVariable;

//
// Console System
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
    std::map<std::string, ConsoleDefinition*> m_definitions;

    bool m_initialized;
};
