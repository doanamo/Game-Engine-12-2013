#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "Game/GameContext.hpp"
#include "Game/Entity/EntitySystem.hpp"

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
    // Process script components.
    for(auto it = Game::ScriptComponents().Begin(); it != Game::ScriptComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!Game::EntitySystem().IsHandleValid(it->first))
            continue;

        // Get the script component.
        ScriptComponent& script = it->second;

        // Call OnCreate() method (only for new components).
        if(!script.m_touched)
        {
            script.OnCreate(it->first);
            script.m_touched = true;
        }

        // Call OnUpdate() method.
        script.OnUpdate(it->first, timeDelta);
    }
}
