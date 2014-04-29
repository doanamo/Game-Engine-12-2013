#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
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

    // Bind event receivers.
    m_receiverEntityDamaged.Bind<ScriptSystem, &ScriptSystem::OnEntityDamagedEvent>(this);
    m_receiverEntityHealed.Bind<ScriptSystem, &ScriptSystem::OnEntityHealedEvent>(this);

    return true;
}

void ScriptSystem::Cleanup()
{
    m_receiverEntityDamaged.Cleanup();
    m_receiverEntityHealed.Cleanup();
}

void ScriptSystem::Update(float timeDelta)
{
    // Process script components.
    for(auto it = GameState::ScriptComponents().Begin(); it != GameState::ScriptComponents().End(); ++it)
    {
        // Check if entity is active.
        if(!GameState::EntitySystem().IsHandleValid(it->first))
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

ReceiverSignature<GameEvent::EntityDamaged> ScriptSystem::GetEntityDamagedReceiver()
{
    return m_receiverEntityDamaged;
}

ReceiverSignature<GameEvent::EntityHealed> ScriptSystem::GetEntityHealedReceiver()
{
    return m_receiverEntityHealed;
}

void ScriptSystem::OnEntityDamagedEvent(const GameEvent::EntityDamaged& event)
{
    // Execute the event script.
    ScriptComponent* script = GameState::ScriptComponents().Lookup(event.entity);

    if(script != nullptr)
    {
        script->OnDamage(event.entity, event.value, event.component->IsAlive());
    }
}

void ScriptSystem::OnEntityHealedEvent(const GameEvent::EntityHealed& event)
{
    // Execute the event script.
    ScriptComponent* script = GameState::ScriptComponents().Lookup(event.entity);

    if(script != nullptr)
    {
        script->OnHeal(event.entity, event.value);
    }
}
