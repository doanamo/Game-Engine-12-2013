#include "Precompiled.hpp"
#include "ScriptSystem.hpp"
#include "ScriptComponent.hpp"
#include "Script.hpp"

#include "Common/Services.hpp"
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
    m_receiverEntityCreated.Cleanup();
    m_receiverEntityDestroyed.Cleanup();
    m_receiverEntityDamaged.Cleanup();
    m_receiverEntityHealed.Cleanup();
    m_receiverEntityCollsion.Cleanup();
}

bool ScriptSystem::Initialize(const Services& services)
{
    Cleanup();

    // Setup scope guard.
    SCOPE_GUARD_IF(!m_initialized, Cleanup());

    // Get required services.
    m_eventSystem = services.Get<EventSystem>();
    if(m_eventSystem == nullptr) return false;

    m_entitySystem = services.Get<EntitySystem>();
    if(m_entitySystem == nullptr) return false;

    m_componentSystem = services.Get<ComponentSystem>();
    if(m_componentSystem == nullptr) return false;

    // Bind event receivers.
    m_receiverEntityCreated.Bind<ScriptSystem, &ScriptSystem::OnEntityCreatedEvent>(this);
    m_receiverEntityDestroyed.Bind<ScriptSystem, &ScriptSystem::OnEntityDestroyedEvent>(this);
    m_receiverEntityDamaged.Bind<ScriptSystem, &ScriptSystem::OnEntityDamagedEvent>(this);
    m_receiverEntityHealed.Bind<ScriptSystem, &ScriptSystem::OnEntityHealedEvent>(this);
    m_receiverEntityCollsion.Bind<ScriptSystem, &ScriptSystem::OnEntityCollisionEvent>(this);

    // Subscribe event receivers.
    m_eventSystem->Subscribe(m_receiverEntityCreated);
    m_eventSystem->Subscribe(m_receiverEntityDestroyed);
    m_eventSystem->Subscribe(m_receiverEntityDamaged);
    m_eventSystem->Subscribe(m_receiverEntityHealed);
    m_eventSystem->Subscribe(m_receiverEntityCollsion);

    // Declare required components.
    m_componentSystem->Declare<ScriptComponent>();
    m_componentSystem->Declare<ScriptLuaComponent>();

    // Success!
    return m_initialized = true;
}

void ScriptSystem::Update(float timeDelta)
{
    assert(m_initialized);

    // Process script components.
    {
        auto componentsBegin = m_componentSystem->Begin<ScriptLuaComponent>();
        auto componentsEnd = m_componentSystem->End<ScriptLuaComponent>();

        for(auto it = componentsBegin; it != componentsEnd; ++it)
        {
            // Check if entity is active.
            if(!m_entitySystem->IsHandleValid(it->first))
                continue;

            // Get the script component.
            ScriptLuaComponent& script = it->second;

            // Call OnUpdate() function.
            script.Call("OnUpdate", it->first, timeDelta);
        }
    }

    // Process script components.
    {
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
}

void ScriptSystem::OnEntityCreatedEvent(const GameEvent::EntityCreated& event)
{
    assert(m_initialized);

    ScriptLuaComponent* script = m_componentSystem->Lookup<ScriptLuaComponent>(event.entity);

    if(script != nullptr)
    {
        script->Call("OnCreated", event.entity);
    }
}

void ScriptSystem::OnEntityDestroyedEvent(const GameEvent::EntityDestroyed& event)
{
    assert(m_initialized);

    ScriptLuaComponent* script = m_componentSystem->Lookup<ScriptLuaComponent>(event.entity);

    if(script != nullptr)
    {
        script->Call("OnDestroyed", event.entity);
    }
}

void ScriptSystem::OnEntityDamagedEvent(const GameEvent::EntityDamaged& event)
{
    assert(m_initialized);

    {
        ScriptLuaComponent* script = m_componentSystem->Lookup<ScriptLuaComponent>(event.entity);

        if(script != nullptr)
        {
            script->Call("OnDamaged", event.entity, event.value, event.component->IsAlive());
        }
    }

    // Execute the event script.
    {
        ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.entity);

        if(script != nullptr)
        {
            script->OnDamage(event.entity, event.value, event.component->IsAlive());
        }
    }
}

void ScriptSystem::OnEntityHealedEvent(const GameEvent::EntityHealed& event)
{
    assert(m_initialized);

    {
        ScriptLuaComponent* script = m_componentSystem->Lookup<ScriptLuaComponent>(event.entity);

        if(script != nullptr)
        {
            script->Call("OnHeal", event.entity, event.value);
        }
    }

    // Execute the event script.
    {
        ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.entity);

        if(script != nullptr)
        {
            script->OnHeal(event.entity, event.value);
        }
    }
}

void ScriptSystem::OnEntityCollisionEvent(const GameEvent::EntityCollision& event)
{
    assert(m_initialized);

    {
        ScriptLuaComponent* script = m_componentSystem->Lookup<ScriptLuaComponent>(event.self.entity);

        if(script != nullptr)
        {
            script->Call("OnCollision", event.self, event.other);
        }
    }

    // Execute the event script.
    {
        ScriptComponent* script = m_componentSystem->Lookup<ScriptComponent>(event.self.entity);

        if(script != nullptr)
        {
            script->OnCollision(event.self, event.other);
        }
    }
}
