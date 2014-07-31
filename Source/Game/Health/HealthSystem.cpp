#include "Precompiled.hpp"
#include "HealthSystem.hpp"
#include "HealthComponent.hpp"

#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"

namespace Console
{
    ConsoleVariable cheatGodMode("godmode", false, "Toggles player invulnerability.");
}

HealthSystem::HealthSystem() :
    m_initialized(false),
    m_eventSystem(nullptr),
    m_entitySystem(nullptr),
    m_identitySystem(nullptr),
    m_componentSystem(nullptr)
{
}

HealthSystem::~HealthSystem()
{
    Cleanup();
}

void HealthSystem::Cleanup()
{
    m_initialized = false;

    m_eventSystem = nullptr;
    m_entitySystem = nullptr;
    m_identitySystem = nullptr;
    m_componentSystem = nullptr;
}

bool HealthSystem::Initialize(EventSystem* eventSystem, EntitySystem* entitySystem, IdentitySystem* identitySystem, ComponentSystem* componentSystem)
{
    Cleanup();

    // Validate arguments.
    if(eventSystem == nullptr)
        return false;

    if(entitySystem == nullptr)
        return false;

    if(identitySystem == nullptr)
        return false;

    if(componentSystem == nullptr)
        return false;

    m_eventSystem = eventSystem;
    m_entitySystem = entitySystem;
    m_identitySystem = identitySystem;
    m_componentSystem = componentSystem;

    // Declare required components.
    m_componentSystem->Declare<HealthComponent>();

    // Success!
    return m_initialized = true;
}

void HealthSystem::Damage(EntityHandle entity, int value)
{
    assert(m_initialized);

    // Check if handle is valid.
    if(!m_entitySystem->IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = m_componentSystem->Lookup<HealthComponent>(entity);

    if(health == nullptr)
        return;

    // Don't damage if dead.
    if(health->IsDead())
        return;

    // Check if it's the player entity.
    bool playerInvulnerable = false;

    if(Console::cheatGodMode)
    {
        EntityHandle player = m_identitySystem->GetEntityByName("Player");

        if(entity == player)
        {
            playerInvulnerable = true;
        }
    }

    // Apply damage.
    if(value > 0 && !playerInvulnerable)
    {
        // Change current health value.
        int currentHealth = health->GetCurrentHealth();
        currentHealth = std::max(0, currentHealth - value);
        health->SetCurrentHealth(currentHealth);

        // Dispatch an entity health event.
        {
            GameEvent::EntityHealth event;
            event.entity = entity;
            event.component = health;

            m_eventSystem->Dispatch(event);
        }

        // Dispatch an entity damaged event.
        {
            GameEvent::EntityDamaged event;
            event.entity = entity;
            event.component = health;
            event.value = value;

            m_eventSystem->Dispatch(event);
        }
    }
}

void HealthSystem::Heal(EntityHandle entity, int value)
{
    assert(m_initialized);

    // Check if handle is valid.
    if(!m_entitySystem->IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = m_componentSystem->Lookup<HealthComponent>(entity);
    if(health == nullptr) return;

    // Don't heal if dead.
    if(health->IsDead())
        return;

    // Apply heal.
    if(value > 0)
    {
        // Change current health value.
        int currentHealth = health->GetCurrentHealth();
        currentHealth = std::min(currentHealth + value, health->GetMaximumHealth());
        health->SetCurrentHealth(currentHealth);

        // Dispatch an entity health event.
        {
            GameEvent::EntityHealth event;
            event.entity = entity;
            event.component = health;

            m_eventSystem->Dispatch(event);
        }

        // Dispatch an entity healed event.
        {
            GameEvent::EntityHealed event;
            event.entity = entity;
            event.component = health;
            event.value = value;

            m_eventSystem->Dispatch(event);
        }
    }
}
