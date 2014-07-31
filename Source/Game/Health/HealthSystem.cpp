#include "Precompiled.hpp"
#include "HealthSystem.hpp"
#include "HealthComponent.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Event/EventDefinitions.hpp"
#include "Game/Event/EventSystem.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"

namespace Console
{
    ConsoleVariable cheatGodMode("godmode", false, "Toggles player invulnerability.");
}

HealthSystem::HealthSystem() :
    m_eventSystem(nullptr)
{
}

HealthSystem::~HealthSystem()
{
    Cleanup();
}

void HealthSystem::Cleanup()
{
    m_eventSystem = nullptr;
}

bool HealthSystem::Initialize(EventSystem* eventSystem)
{
    Cleanup();

    // Validate arguments.
    if(eventSystem == nullptr)
        return false;

    m_eventSystem = eventSystem;

    return true;
}

void HealthSystem::Damage(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!GameState::GetEntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::GetComponentSystem().Lookup<HealthComponent>(entity);
    if(health == nullptr) return;

    // Don't damage if dead.
    if(health->IsDead())
        return;

    // Check if it's the player entity.
    bool playerInvulnerable = false;

    if(Console::cheatGodMode)
    {
        EntityHandle player = GameState::GetIdentitySystem().GetEntityByName("Player");

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
    // Check if handle is valid.
    if(!GameState::GetEntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::GetComponentSystem().Lookup<HealthComponent>(entity);
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
