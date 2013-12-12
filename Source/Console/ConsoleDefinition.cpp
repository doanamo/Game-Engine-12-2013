#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"
#include "ConsoleSystem.hpp"

ConsoleDefinition*	ConsoleDefinition::m_staticHead = nullptr;
bool				ConsoleDefinition::m_staticDone = false;

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
		Context::consoleSystem->RegisterDefinition(this);
	}
}

ConsoleDefinition::~ConsoleDefinition()
{
	// Unregister the definition.
	if(Context::consoleSystem != nullptr)
	{
		Context::consoleSystem->UnregisterDefinition(this);
	}
}

void ConsoleDefinition::RegisterStatic()
{
	assert(Context::consoleSystem != nullptr);

	// Register all definitions that were created before the console system initialization.
	for(auto definition = m_staticHead; definition != nullptr; definition = definition->m_staticNext)
	{
		Context::consoleSystem->RegisterDefinition(definition);
	}

	// Static definitions initialized.
	m_staticDone = true;
}
