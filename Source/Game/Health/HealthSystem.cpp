#include "Precompiled.hpp"
#include "HealthSystem.hpp"
#include "HealthComponent.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"

namespace Console
{
    ConsoleVariable cheatGodMode("godmode", false, "Toggles player invulnerability.");
}

HealthSystem::HealthSystem()
{
}

HealthSystem::~HealthSystem()
{
    Cleanup();
}

bool HealthSystem::Initialize()
{
    return true;
}

void HealthSystem::Cleanup()
{
    m_dispatcherEntityHealth.Cleanup();
    m_dispatcherEntityDamaged.Cleanup();
    m_dispatcherEntityHealed.Cleanup();
}

void HealthSystem::Damage(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!GameState::GetEntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::GetHealthComponents().Lookup(entity);
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

            m_dispatcherEntityHealth.Dispatch(event);
        }

        // Dispatch an entity damaged event.
        {
            GameEvent::EntityDamaged event;
            event.entity = entity;
            event.component = health;
            event.value = value;

            m_dispatcherEntityDamaged.Dispatch(event);
        }
    }
}

void HealthSystem::Heal(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!GameState::GetEntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::GetHealthComponents().Lookup(entity);
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

            m_dispatcherEntityHealth.Dispatch(event);
        }

        // Dispatch an entity healed event.
        {
            GameEvent::EntityHealed event;
            event.entity = entity;
            event.component = health;
            event.value = value;

            m_dispatcherEntityHealed.Dispatch(event);
        }
    }
}

void HealthSystem::SubscribeReceiver(const ReceiverSignature<GameEvent::EntityHealth>& signature)
{
    m_dispatcherEntityHealth.Subscribe(signature);
}

void HealthSystem::SubscribeReceiver(const ReceiverSignature<GameEvent::EntityDamaged>& signature)
{
    m_dispatcherEntityDamaged.Subscribe(signature);
}

void HealthSystem::SubscribeReceiver(const ReceiverSignature<GameEvent::EntityHealed>& signature)
{
    m_dispatcherEntityHealed.Subscribe(signature);
}
