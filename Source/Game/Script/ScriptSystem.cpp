#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"

ScriptSystem::ScriptSystem()
{
}

ScriptSystem::~ScriptSystem()
{
    Cleanup();
}

void ScriptSystem::Cleanup()
{
    m_receiverEntityDamaged.Cleanup();
    m_receiverEntityHealed.Cleanup();
    m_receiverEntityCollsion.Cleanup();
}

bool ScriptSystem::Initialize(EventSystem* eventSystem)
{
    Cleanup();

    // Validate arguments.
    if(eventSystem == nullptr)
        return false;

    // Bind event receivers.
    m_receiverEntityDamaged.Bind<ScriptSystem, &ScriptSystem::OnEntityDamagedEvent>(this);
    m_receiverEntityHealed.Bind<ScriptSystem, &ScriptSystem::OnEntityHealedEvent>(this);
    m_receiverEntityCollsion.Bind<ScriptSystem, &ScriptSystem::OnEntityCollisionEvent>(this);

    // Subscribe event receivers.
    eventSystem->Subscribe(&m_receiverEntityDamaged);
    eventSystem->Subscribe(&m_receiverEntityHealed);
    eventSystem->Subscribe(&m_receiverEntityCollsion);

    return true;
}

void ScriptSystem::Update(float timeDelta)
{
    // Process script components.
    for(auto it = GameState::GetComponentSystem().Begin<ScriptComponent>(); it != GameState::GetComponentSystem().End<ScriptComponent>(); ++it)
    {
        // Check if entity is active.
        if(!GameState::GetEntitySystem().IsHandleValid(it->first))
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

void ScriptSystem::OnEntityDamagedEvent(const GameEvent::EntityDamaged& event)
{
    // Execute the event script.
    ScriptComponent* script = GameState::GetComponentSystem().Lookup<ScriptComponent>(event.entity);

    if(script != nullptr)
    {
        script->OnDamage(event.entity, event.value, event.component->IsAlive());
    }
}

void ScriptSystem::OnEntityHealedEvent(const GameEvent::EntityHealed& event)
{
    // Execute the event script.
    ScriptComponent* script = GameState::GetComponentSystem().Lookup<ScriptComponent>(event.entity);

    if(script != nullptr)
    {
        script->OnHeal(event.entity, event.value);
    }
}

void ScriptSystem::OnEntityCollisionEvent(const GameEvent::EntityCollision& event)
{
    // Execute the event script.
    ScriptComponent* script = GameState::GetComponentSystem().Lookup<ScriptComponent>(event.self.entity);

    if(script != nullptr)
    {
        script->OnCollision(event.self, event.other);
    }
}
