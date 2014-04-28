#include "Precompiled.hpp"
#include "PlayerScript.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Input/InputState.hpp"
#include "Game/Input/InputComponent.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionDefinitions.hpp"
#include "Game/Collision/CollisionComponent.hpp"

PlayerScript::PlayerScript() :
    m_shootTime(0.0f)
{
}

void PlayerScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Check if entity has needed components.
    TransformComponent* transform = GameState::TransformComponents().Lookup(self);
    if(transform == nullptr) return;

    CollisionComponent* collision = GameState::CollisionComponents().Lookup(self);
    if(collision == nullptr) return;

    InputComponent* input = GameState::InputComponents().Lookup(self);
    if(input == nullptr) return;

    // Shoot a projectile.
    m_shootTime = std::max(0.0f, m_shootTime - timeDelta);

    if(input->GetState()->IsKeyDown(SDL_SCANCODE_SPACE))
    {
        if(m_shootTime == 0.0f)
        {
            // Create a projectile entity.
            GameFactory::CreateProjectile(transform->GetPosition(), glm::vec2(1.0f, 0.0f), 700, CollisionTypes::Enemy | CollisionTypes::Environment);

            // Set a shooting delay.
            m_shootTime = 0.2f;
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
        // Calculate new position.
        glm::vec2 position = transform->GetPosition();
        position += glm::normalize(direction) * 400.0f * timeDelta;

        // Prevent from moving out of the playable area.
        glm::vec4 boundingBox = collision->GetBoundingBox();
        position.x = Clamp(position.x, 0.0f - boundingBox.x, 1024.0f - boundingBox.w);
        position.y = Clamp(position.y, 0.0f - boundingBox.y, 576.0f - boundingBox.z);

        // Set new position.
        transform->SetPosition(position);
    }
}

void PlayerScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        GameState::EntitySystem().DestroyEntity(self);
    }
}
