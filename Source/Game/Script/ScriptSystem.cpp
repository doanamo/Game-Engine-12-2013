#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"

ScriptSystem::ScriptSystem() :
    m_initialized(false),
    m_eventSystem(nullptr),
    m_entitySystem(nullptr),
    m_componentSystem(nullptr)
{
}

ScriptSystem::~ScriptSystem()
{
    Cleanup();
}

void ScriptSystem::Cleanup()
{
    m_initialized = false;

    // Game systems.
    m_eventSystem = nullptr;
    m_entitySystem = nullptr;
    m_componentSystem = nullptr;

    // Event receivers.
    m_receiverEntityDamaged.Cleanup();
    m_receiverEntityHealed.Cleanup();
    m_receiverEntityCollsion.Cleanup();
}

bool ScriptSystem::Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, ComponentSystem* componentSystem)
{
    Cleanup();

    // Validate arguments.
    if(eventSystem == nullptr)
        return false;

    if(entitySystem == nullptr)
        return false;

    if(componentSystem == nullptr)
        return false;

    m_eventSystem = eventSystem;
    m_entitySystem = entitySystem;
    m_componentSystem = componentSystem;

    // Bind event receivers.
    m_receiverEntityDamaged.Bind<ScriptSystem, &ScriptSystem::OnEntityDamagedEvent>(this);
    m_receiverEntityHealed.Bind<ScriptSystem, &ScriptSystem::OnEntityHealedEvent>(this);
    m_receiverEntityCollsion.Bind<ScriptSystem, &ScriptSystem::OnEntityCollisionEvent>(this);

    // Subscribe event receivers.
    eventSystem->Subscribe(&m_receiverEntityDamaged);
    eventSystem->Subscribe(&m_receiverEntityHealed);
    eventSystem->Subscribe(&m_receiverEntityCollsion);

    // Declare required components.
    m_componentSystem->Declare<ScriptComponent>();

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    assert(m_initialized);

    // Process script components.
    auto componentsBegin = m_componentSystem->Begin<ScriptComponent>();
    auto componentsEnd = m_componentSystem->End<ScriptComponent>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Check if entity is active.
        if(!m_entitySystem->IsHandleValid(it->first))
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
    assert(m_initialized);

    // Execute the event script.
    ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.entity);

    if(script != nullptr)
    {
        script->OnDamage(event.entity, event.value, event.component->IsAlive());
    }
}

void ScriptSystem::OnEntityHealedEvent(const GameEvent::EntityHealed& event)
{
    assert(m_initialized);

    // Execute the event script.
    ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.entity);

    if(script != nullptr)
    {
        script->OnHeal(event.entity, event.value);
    }
}

void ScriptSystem::OnEntityCollisionEvent(const GameEvent::EntityCollision& event)
{
    assert(m_initialized);

    // Execute the event script.
    ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.self.entity);

    if(script != nullptr)
    {
        script->OnCollision(event.self, event.other);
    }
}
