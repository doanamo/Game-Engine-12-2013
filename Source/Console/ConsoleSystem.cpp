#include "Precompiled.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleDefinition.hpp"
#include "ConsoleVariable.hpp"

ConsoleSystem::ConsoleSystem() :
    m_initialized(false)
{
}

ConsoleSystem::~ConsoleSystem()
{
    Cleanup();
}

bool ConsoleSystem::Initialize()
{
    Cleanup();

    m_initialized = true;

    return true;
}

void ConsoleSystem::Cleanup()
{
    // Clear all definitions.
    ClearContainer(m_definitions);

    m_initialized = false;
}

void ConsoleSystem::Execute(std::string input)
{
    if(!m_initialized)
        return;

    // Print input string.
    Log() << "> " << input;

    // Abort if input is empty.
    if(input.empty())
        return;

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

    if(!m_initialized)
        return;

    if(definition == nullptr)
        return;

    // Definition name can't be empty.
    assert(definition->GetName() != "");

    // Check if a definition is meant to be used as internal.
    if(definition->GetName() == ConsoleDefinition::Internal)
        return;

    // White spaces (and special character) may cause problems.
    assert(definition->GetName().find(' ') == std::string::npos);

    // Register a definition.
    auto result = m_definitions.insert(std::make_pair(definition->GetName(), definition));

    assert(result.second == true);
}

void ConsoleSystem::UnregisterDefinition(ConsoleDefinition* definition)
{
    if(!m_initialized)
        return;

    // Unregister a definition.
    auto result = m_definitions.erase(definition->GetName());

    assert(result == 1);
}

ConsoleDefinition* ConsoleSystem::FindDefinition(std::string name)
{
    if(!m_initialized)
        return nullptr;

    // Find the definition by name.
    auto it = m_definitions.find(name);
    if(it == m_definitions.end())
        return nullptr;

    return it->second;
}
