#include "Precompiled.hpp"
#include "PlayerScript.hpp"

#include "InputState.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"
#include "EntitySystem.hpp"

#include "CollisionTypes.hpp"
#include "TransformComponent.hpp"
#include "InputComponent.hpp"

PlayerScript::PlayerScript() :
    m_shootTime(0.0f)
{
}

void PlayerScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = Game::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    InputComponent* input = Game::InputComponents().Lookup(self);
    if(input == nullptr) return;

    // Shoot a projectile.
    m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_SPACE))
    {
        if(m_shootTime == 0.0f)
        {
            // Create a projectile entity.
            Game::CreateProjectile(transform->GetPosition(), glm::vec2(1.0f, 0.0f), 700, CollisionTypes::Enemy);

            // Set a shooting delay.
            m_shootTime = 0.34f;
        }
    }

    // Create a direction vector.
    glm::vec2 direction(0.0f, 0.0f);

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_D))
    {
        direction.x = 1.0f;
    }

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_A))
    {
        direction.x = -1.0f;
    }

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_W))
    {
        direction.y = 1.0f;
    }

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_S))
    {
        direction.y = -1.0f;
    }

    // Update player position.
    if(direction != glm::vec2(0.0f))
    {
        glm::vec2 position = transform->GetPosition();
        position += glm::normalize(direction) * 400.0f * timeDelta;
        transform->SetPosition(position);
    }
}

void PlayerScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        Game::EntitySystem().DestroyEntity(self);
    }
}
