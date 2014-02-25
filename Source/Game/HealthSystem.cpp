#include "Precompiled.hpp"
#include "HealthSystem.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"

#include "HealthComponent.hpp"
#include "ScriptComponent.hpp"

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
}

void HealthSystem::Damage(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!Game::EntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = Game::HealthComponents().Lookup(entity);
    if(health == nullptr) return;

    // Don't damage if dead.
    if(health->IsDead())
        return;

    // Apply damage.
    if(value > 0)
    {
        // Change current health value.
        int currentHealth = health->GetCurrentHealth();
        currentHealth = std::max(0, currentHealth - value);
        health->SetCurrentHealth(currentHealth);

        // Inform about received damage.
        ScriptComponent* script = Game::ScriptComponents().Lookup(entity);

        if(script != nullptr)
        {
            script->OnDamage(entity, value, health->IsAlive());
        }
    }
}

void HealthSystem::Heal(EntityHandle entity, int value)
{
    // Check if handle is valid.
    if(!Game::EntitySystem().IsHandleValid(entity))
        return;

    // Get the health component.
    HealthComponent* health = Game::HealthComponents().Lookup(entity);
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
        ScriptComponent* script = Game::ScriptComponents().Lookup(entity);

        if(script != nullptr)
        {
            script->OnHeal(entity, value);
        }
    }
}
