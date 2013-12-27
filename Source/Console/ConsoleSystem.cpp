#include "Precompiled.hpp"
#include "ConsoleSystem.hpp"

ConsoleSystem::ConsoleSystem()
{
}

ConsoleSystem::~ConsoleSystem()
{
	Cleanup();
}

bool ConsoleSystem::Initialize()
{
	Cleanup();

	return true;
}

void ConsoleSystem::Cleanup()
{
	// Clear all definitions.
	ClearContainer(m_definitions);
}

void ConsoleSystem::Execute(std::string input)
{
	// Locate the first white space.
	std::size_t separator = input.find(' ');

	// Get the definition name.
	std::string name(input);

	if(separator != std::string::npos)
	{
		name.erase(separator);
	}

	// Find the definition.
	auto it = m_definitions.find(name);
	if(it == m_definitions.end())
	{
		Log() << "Couldn't find \"" << name << "\" definition.";
		return;
	}

	// Get the arguments.
	std::string arguments;

	if(separator != std::string::npos)
	{
		arguments = std::string(input, separator + 1);
	}

	// Execute the definition.
	ConsoleDefinition* definition = it->second;
	definition->Execute(arguments);
}

void ConsoleSystem::RegisterDefinition(ConsoleDefinition* definition)
{
	assert(definition != nullptr);

	// White spaces (and special character) may cause problems.
	assert(definition->GetName().find(' ') == std::string::npos);

	// Register a definition.
	auto result = m_definitions.insert(std::make_pair(definition->GetName(), definition));

	assert(result.second == true);
}

void ConsoleSystem::UnregisterDefinition(ConsoleDefinition* definition)
{
	// Unregister a definition.
	auto result = m_definitions.erase(definition->GetName());

	assert(result == 1);
}

ConsoleDefinition* ConsoleSystem::FindDefinition(std::string name)
{
	// Find the definition by name.
	auto it = m_definitions.find(name);
	if(it == m_definitions.end())
		return nullptr;

	return it->second;
}
