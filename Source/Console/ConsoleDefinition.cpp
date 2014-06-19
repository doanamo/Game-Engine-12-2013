#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"

#include "MainContext.hpp"
#include "Console/ConsoleSystem.hpp"

const char*        ConsoleDefinition::Internal = "";
ConsoleDefinition* ConsoleDefinition::m_staticHead = nullptr;
bool               ConsoleDefinition::m_staticDone = false;

ConsoleDefinition::ConsoleDefinition(std::string name, std::string description) :
    m_name(name),
    m_description(description),
    m_executed(false),
    m_changed(false),
    m_staticNext(nullptr)
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

void ConsoleDefinition::Executed()
{
    m_executed = true;
}

void ConsoleDefinition::Changed()
{
    m_changed = true;
}

void ConsoleDefinition::ResetIntermediateState()
{
    m_executed = false;
    m_changed = false;
}

void ConsoleDefinition::FinalizeStatic()
{
    m_staticDone = true;
}
