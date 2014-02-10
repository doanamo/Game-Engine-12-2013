#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
    Cleanup();
}

bool ScriptSystem::Initialize()
{
    Cleanup();

    return true;
}

void ScriptSystem::Cleanup()
{
}

void ScriptSystem::Update(float timeDelta)
{
    // Touch newly created script components.
    for(auto it = Game::ScriptComponents().Begin(); it != Game::ScriptComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the script component.
        ScriptComponent& script = it->second;

        // Call OnCreate() script function.
        if(script.m_script != nullptr)
        {
            if(!script.m_script->m_touched)
            {
                script.m_script->OnCreate(it->first);
                script.m_script->m_touched = true;
            }
        }
    }

    // Process script components.
    for(auto it = Game::ScriptComponents().Begin(); it != Game::ScriptComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the script component.
        ScriptComponent& script = it->second;

        // Execute the script.
        if(script.m_script != nullptr)
        {
            script.m_script->OnUpdate(it->first, timeDelta);
        }
    }
}
