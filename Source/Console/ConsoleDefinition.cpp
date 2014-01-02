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
		Context::ConsoleSystem().RegisterDefinition(this);
	}
}

ConsoleDefinition::~ConsoleDefinition()
{
	// Unregister the definition.
	// If the definition is static, it could have already
	// been unregistered on console system shutdown.
	Context::ConsoleSystem().UnregisterDefinition(this);
}

void ConsoleDefinition::RegisterStatic()
{
	assert(Context::ConsoleSystem().IsValid());

	// Register all definitions that were created before the console system initialization.
	for(auto definition = m_staticHead; definition != nullptr; definition = definition->m_staticNext)
	{
		Context::ConsoleSystem().RegisterDefinition(definition);
	}

	// Static definitions initialized.
	m_staticDone = true;
}
