#include "Precompiled.hpp"
#include "ScriptComponent.hpp"

ScriptComponent::ScriptComponent()
{
}

ScriptComponent::~ScriptComponent()
{
}

void ScriptComponent::AddScript(Lua::LuaRef script)
{
    // Check if script is valid.
    if(script.isNil())
    {
        Log() << "Error: Passed a nil script.";
        return;
    }

    // Must be a table.
    if(!script.isTable())
    {
        Log() << "Error: Passed an invalid script.";
        return;
    }

    // Add script to the list.
    m_scripts.push_back(script);
}
