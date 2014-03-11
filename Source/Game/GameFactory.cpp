#include "Precompiled.hpp"
#include "GameFactory.hpp"

#include "GameContext.hpp"
#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"

#include "CollisionDefinitions.hpp"

#include "TransformComponent.hpp"
#include "InputComponent.hpp"
#include "HealthComponent.hpp"
#include "CollisionComponent.hpp"
#include "ScriptComponent.hpp"
#include "RenderComponent.hpp"

#include "CommonScripts.hpp"
#include "PlayerScript.hpp"
#include "EnemyScript.hpp"
#include "ProjectileScript.hpp"
#include "HealthPickupScript.hpp"
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

    // Create entity bounds.
    {
        EntityHandle entity = Game::EntitySystem().CreateEntity();

        TransformComponent* transform = Game::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        CollisionComponent* collision = Game::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 1024.0f + 200.0f, 576.0f));
        collision->SetType(CollisionTypes::None);
        collision->SetMask(CollisionMasks::All ^ CollisionTypes::Player);
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
    collision->SetMask(CollisionTypes::Enemy);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<PlayerScript>());
    script->AddScript(std::make_shared<DamageOnCollision>(10, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    return entity;
}

namespace
{
    std::random_device deviceRandom;
    std::mt19937 asteroidRandom(deviceRandom());
}

EntityHandle Game::CreateAsteroid(const glm::vec2& position)
{
    // Calculate entity parameters.
    std::array<double, 3> intervals = { 0.2f, 0.3f, 1.0f };
    std::array<double, 3> weights = { 0.0f, 0.6f, 1.0f };

    float scale = std::piecewise_linear_distribution<float>(intervals.begin(), intervals.end(), weights.begin())(asteroidRandom);

    float size = 200.0f * scale;
    float halfSize = size * 0.5f;

    int hitpoints = std::max(10, (int)(200 * scale));

    float speed = 400.0f - 300.0f * scale;

    // Create an entity.
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(size, size));
    transform->SetRotation(0.0f);

    HealthComponent* health = Game::HealthComponents().Create(entity);
    health->SetMaximumHealth(hitpoints);
    health->SetCurrentHealth(hitpoints);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-halfSize, -halfSize, halfSize, halfSize));
    collision->SetType(CollisionTypes::Environment);
    collision->SetMask(CollisionTypes::Player | CollisionTypes::Enemy);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(glm::vec2(-speed, 0.0f)));
    script->AddScript(std::make_shared<DamageOnCollision>(5, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());
    script->AddScript(std::make_shared<DestroyOnDeathScript>());

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.6f, 0.3f, 0.0f, 1.0f));

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
    health->SetMaximumHealth(30);
    health->SetCurrentHealth(30);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Enemy);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<EnemyScript>());
    script->AddScript(std::make_shared<DamageOnCollision>(5, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

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
    render->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    return entity;
};

EntityHandle Game::CreateHealthPickup(const glm::vec2& position, int heal)
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(40.0f, 40.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = Game::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-20.0f, -20.0f, 20.0f, 20.0f));
    collision->SetType(CollisionTypes::Pickup);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<HealthPickupScript>(20));

    RenderComponent* render = Game::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.6f, 1.0f, 0.6f, 1.0f));

    return entity;
}

EntityHandle Game::CreateSpawner(const glm::vec2& position)
{
    EntityHandle entity = Game::EntitySystem().CreateEntity();

    TransformComponent* transform = Game::TransformComponents().Create(entity);
    transform->SetPosition(position);

    ScriptComponent* script = Game::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<SpawnerScript>());

    return entity;
}
