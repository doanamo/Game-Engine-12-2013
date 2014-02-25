#include "Precompiled.hpp"
#include "GameFactory.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"

#include "CollisionTypes.hpp"
#include "TransformComponent.hpp"
#include "InputComponent.hpp"
#include "HealthComponent.hpp"
#include "CollisionComponent.hpp"
#include "ScriptComponent.hpp"
#include "RenderComponent.hpp"

#include "PlayerScript.hpp"
#include "EnemyScript.hpp"
#include "ProjectileScript.hpp"
#include "SpawnerScript.hpp"

void Game::CreateBounds()
{
    // Bounds script.
    class BoundsScript : public Script
    {
    public:
        void OnCollision(CollisionObject& self, CollisionObject& other)
        {
            assert(other.collision != nullptr);

            // Destroy entity.
            Game::EntitySystem().DestroyEntity(other.entity);
        }
    };

    // Create projectile bounds.
    {
        EntityHandle entity = Game::EntitySystem().CreateEntity();

        TransformComponent* transform = Game::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        CollisionComponent* collision = Game::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 1024.0f, 576.0f));
        collision->SetType(CollisionTypes::None);
        collision->SetMask(CollisionTypes::Projectile);
        collision->SetFlags(CollisionFlags::Default | CollisionFlags::Reversed);

        ScriptComponent* script = Game::ScriptComponents().Create(entity);
        script->AddScript(std::make_shared<BoundsScript>());
    }

    // Create enemy bounds.
    {
        EntityHandle entity = Game::EntitySystem().CreateEntity();

        TransformComponent* transform = Game::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        CollisionComponent* collision = Game::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 1024.0f + 300.0f, 576.0f));
        collision->SetType(CollisionTypes::None);
        collision->SetMask(CollisionTypes::Enemy);
        collision->SetFlags(CollisionFlags::Default | CollisionFlags::Reversed);

        ScriptComponent* script = Game::ScriptComponents().Create(entity);
        script->AddScript(std::make_shared<BoundsScript>());
    }
}

EntityHandle Game::CreatePlayer()
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(glm::vec2(50.0f, 275.0f));
    transform->SetScale(glm::vec2(50.0f, 50.0f));
    transform->SetRotation(0.0f);

    InputComponent* input = Game::InputComponents().Create(entity);
    input->SetStateReference(&Game::InputState());

    HealthComponent* health = Game::HealthComponents().Create(entity);
    health->SetMaximumHealth(100);
    health->SetCurrentHealth(100);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Player);
    collision->SetMask(CollisionTypes::None);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<PlayerScript>());

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    return entity;
}

EntityHandle Game::CreateEnemy(const glm::vec2& position)
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(50.0f, 50.0f));
    transform->SetRotation(0.0f);

    HealthComponent* health = Game::HealthComponents().Create(entity);
    health->SetMaximumHealth(20);
    health->SetCurrentHealth(20);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Enemy);
    collision->SetMask(CollisionTypes::None);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<EnemyScript>());

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    return entity;
}

EntityHandle Game::CreateProjectile(const glm::vec2& position, const glm::vec2& direction, float speed, uint32_t collisionMask)
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(40.0f, 40.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-20.0f, -20.0f, 20.0f, 20.0f));
    collision->SetType(CollisionTypes::Projectile);
    collision->SetMask(collisionMask);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<ProjectileScript>(direction, speed));

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    return entity;
};

EntityHandle Game::CreateSpawner(const glm::vec2& position)
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<SpawnerScript>());

    return entity;
}
