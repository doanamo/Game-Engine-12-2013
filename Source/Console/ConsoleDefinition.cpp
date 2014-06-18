#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"
#include "ConsoleSystem.hpp"

#include "MainContext.hpp"

const char*        ConsoleDefinition::Internal = "internal";
ConsoleDefinition* ConsoleDefinition::m_staticHead = nullptr;
bool               ConsoleDefinition::m_staticDone = false;

ConsoleDefinition::ConsoleDefinition(std::string name, std::string description) :
    m_name(name), m_description(description), m_staticNext(nullptr)
{
    if(!m_staticDone)
    {
        // Add static definition to the list.
        m_staticNext = m_staticHead;
        m_staticHead = this;
    }
    else
    {
        // Register a definition.
        Main::ConsoleSystem().RegisterDefinition(this);
    }
}

ConsoleDefinition::~ConsoleDefinition()
{
    // Unregister the definition.
    // If the definition is static, it could have already been
    // unregistered on console system shutdown, which is fine.
    Main::ConsoleSystem().UnregisterDefinition(this);
}

void ConsoleDefinition::FinalizeStatic()
{
    m_staticDone = true;
}
