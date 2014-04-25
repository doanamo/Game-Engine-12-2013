#include "Precompiled.hpp"
#include "HealthSystem.hpp"
#include "HealthComponent.hpp"

#include "Game/GameContext.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Script/ScriptComponent.hpp"
#include "Game/Interface/InterfaceSystem.hpp"

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

    // Check if it's the player entity.
    bool playerInvulnerable = false;

    if(Console::cheatGodMode)
    {
        EntityHandle player = Game::IdentitySystem().GetEntityByName("Player");
        playerInvulnerable = entity == player;
    }

    // Apply damage.
    if(value > 0 && !playerInvulnerable)
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

    // Add a floating damage number.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    
    if(transform != nullptr)
    {
        std::string text = std::to_string(value);
        Game::InterfaceSystem().AddFloatingText(text, transform->GetPosition(), &FloatingDamageNumber::GetInstance());
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

    // Add a floating heal number.
    TransformComponent* transform = Game::TransformComponents().Lookup(entity);
    
    if(transform != nullptr)
    {
        std::string text = std::to_string(value);
        Game::InterfaceSystem().AddFloatingText(text, transform->GetPosition(), &FloatingHealNumber::GetInstance());
    }
}
