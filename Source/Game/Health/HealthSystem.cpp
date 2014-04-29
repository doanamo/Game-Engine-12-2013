#include "Precompiled.hpp"
#include "HealthSystem.hpp"
#include "HealthComponent.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Script/ScriptComponent.hpp"

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
    m_dispatcherHealthChange.Cleanup();
}

void HealthSystem::Damage(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!GameState::EntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::HealthComponents().Lookup(entity);
    if(health == nullptr) return;

    // Don't damage if dead.
    if(health->IsDead())
        return;

    // Check if it's the player entity.
    bool playerInvulnerable = false;

    if(Console::cheatGodMode)
    {
        EntityHandle player = GameState::IdentitySystem().GetEntityByName("Player");

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

        // Inform about received damage.
        ScriptComponent* script = GameState::ScriptComponents().Lookup(entity);

        if(script != nullptr)
        {
            script->OnDamage(entity, value, health->IsAlive());
        }

        // Dispatch a health change event.
        HealthChangeEvent event;
        event.entity = entity;
        event.component = health;
        event.value = -value;

        m_dispatcherHealthChange.Dispatch(event);
    }
}

void HealthSystem::Heal(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!GameState::EntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = GameState::HealthComponents().Lookup(entity);
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

        // Inform about received heal.
        ScriptComponent* script = GameState::ScriptComponents().Lookup(entity);

        if(script != nullptr)
        {
            script->OnHeal(entity, value);
        }

        // Dispatch a health change event.
        HealthChangeEvent event;
        event.entity = entity;
        event.component = health;
        event.value = value;

        m_dispatcherHealthChange.Dispatch(event);
    }
}

void HealthSystem::SubscribeReceiver(const ReceiverSignature<HealthChangeEvent>& signature)
{
    m_dispatcherHealthChange.Subscribe(signature);
}
