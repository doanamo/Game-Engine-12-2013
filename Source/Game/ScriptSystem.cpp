#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "Script.hpp"
#include "EntitySystem.hpp"
#include "Entity.hpp"

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

void ScriptSystem::Process(Entity* entity)
{
    // Check if the entity contains needed component.
    Script* script = entity->GetComponent<Script>();

    if(script == nullptr)
        return;

    // Execute script function.
    if(script->m_object != nullptr)
    {
        script->m_object->Execute(entity, GetEntitySystem()->GetTimeDelta());
    }
}
