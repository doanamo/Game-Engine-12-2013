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
    // Add static definition to the list.
    if(m_staticDone == false)
    {
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
    // If the definition is static, it could have already
    // been unregistered on console system shutdown.
    Main::ConsoleSystem().UnregisterDefinition(this);
}

void ConsoleDefinition::RegisterStatic()
{
    assert(Main::ConsoleSystem().IsValid());

    // Register all definitions that were created before the console system initialization.
    for(auto definition = m_staticHead; definition != nullptr; definition = definition->m_staticNext)
    {
        Main::ConsoleSystem().RegisterDefinition(definition);
    }

    // Static definitions initialized.
    m_staticDone = true;
}
