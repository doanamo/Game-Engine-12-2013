#include "Precompiled.hpp"
#include "GameFactory.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputComponent.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionObject.hpp"
#include "Game/Collision/CollisionComponent.hpp"
#include "Game/Collision/CollisionDefinitions.hpp"
#include "Game/Health/HealthComponent.hpp"
#include "Game/Script/ScriptComponent.hpp"
#include "Game/Script/CommonScripts.hpp"
#include "Game/Script/PlayerScript.hpp"
#include "Game/Script/EnemyScript.hpp"
#include "Game/Script/ProjectileScript.hpp"
#include "Game/Script/HealthPickupScript.hpp"
#include "Game/Script/SpawnerScript.hpp"
#include "Game/Render/RenderComponent.hpp"

namespace
{
    std::random_device deviceRandom;
}

void GameFactory::CreateBounds()
{
    // Create projectile bounds.
    {
        EntityHandle entity = GameState::EntitySystem().CreateEntity();

        TransformComponent* transform = GameState::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 1024.0f, 576.0f));
        collision->SetType(CollisionTypes::None);
        collision->SetMask(CollisionTypes::Projectile);
        collision->SetFlags(CollisionFlags::Default | CollisionFlags::Reversed);

        ScriptComponent* script = GameState::ScriptComponents().Create(entity);
        script->AddScript(std::make_shared<DestroyOnCollision>());
    }

    // Create entity bounds.
    {
        EntityHandle entity = GameState::EntitySystem().CreateEntity();

        TransformComponent* transform = GameState::TransformComponents().Create(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
        collision->SetBoundingBox(glm::vec4(0.0f, 0.0f, 1024.0f + 200.0f, 576.0f));
        collision->SetType(CollisionTypes::None);
        collision->SetMask(CollisionMasks::All ^ CollisionTypes::Player);
        collision->SetFlags(CollisionFlags::Default | CollisionFlags::Reversed);

        ScriptComponent* script = GameState::ScriptComponents().Create(entity);
        script->AddScript(std::make_shared<DestroyOnCollision>());
    }
}

EntityHandle GameFactory::CreatePlayer()
{
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    GameState::IdentitySystem().SetEntityName(entity, "Player");

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(glm::vec2(50.0f, 275.0f));
    transform->SetScale(glm::vec2(50.0f, 50.0f));
    transform->SetRotation(0.0f);

    InputComponent* input = GameState::InputComponents().Create(entity);
    input->SetStateReference(&GameState::InputState());

    HealthComponent* health = GameState::HealthComponents().Create(entity);
    health->SetMaximumHealth(100);
    health->SetCurrentHealth(100);

    CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Player);
    collision->SetMask(CollisionTypes::Enemy);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<PlayerScript>());
    script->AddScript(std::make_shared<DamageOnCollision>(10, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());

    RenderComponent* render = GameState::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    return entity;
}

namespace
{
    std::mt19937 asteroidRandom(deviceRandom());
}

EntityHandle GameFactory::CreateAsteroid(const glm::vec2& position)
{
    // Calculate entity parameters.
    std::array<double, 3> intervals = { 0.2f, 0.3f, 1.0f };
    std::array<double, 3> weights = { 0.0f, 0.6f, 1.0f };

    float scale = std::piecewise_linear_distribution<float>(intervals.begin(), intervals.end(), weights.begin())(asteroidRandom);

    float size = 200.0f * scale;
    float halfSize = size * 0.5f;

    int hitpoints = std::max(10, (int)(160 * scale));

    float speed = 400.0f - 300.0f * scale;

    // Create an entity.
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(size, size));
    transform->SetRotation(0.0f);

    HealthComponent* health = GameState::HealthComponents().Create(entity);
    health->SetMaximumHealth(hitpoints);
    health->SetCurrentHealth(hitpoints);

    CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-halfSize, -halfSize, halfSize, halfSize));
    collision->SetType(CollisionTypes::Environment);
    collision->SetMask(CollisionTypes::Player | CollisionTypes::Enemy);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(glm::vec2(-speed, 0.0f)));
    script->AddScript(std::make_shared<DamageOnCollision>(5, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());
    script->AddScript(std::make_shared<DestroyOnDeathScript>());

    RenderComponent* render = GameState::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.6f, 0.3f, 0.0f, 1.0f));

    return entity;
}

namespace
{
    std::mt19937 enemyRandom(deviceRandom());
}

EntityHandle GameFactory::CreateEnemy(const glm::vec2& position)
{
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(50.0f, 50.0f));
    transform->SetRotation(0.0f);

    HealthComponent* health = GameState::HealthComponents().Create(entity);
    health->SetMaximumHealth(30);
    health->SetCurrentHealth(30);

    CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Enemy);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<EnemyScript>());
    script->AddScript(std::make_shared<DamageOnCollision>(5, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());

    RenderComponent* render = GameState::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    return entity;
}

EntityHandle GameFactory::CreateProjectile(const glm::vec2& position, const glm::vec2& direction, float speed, uint32_t collisionMask)
{
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(30.0f, 30.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-15.0f, -15.0f, 15.0f, 15.0f));
    collision->SetType(CollisionTypes::Projectile);
    collision->SetMask(collisionMask);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(direction * speed));
    script->AddScript(std::make_shared<ProjectileScript>());

    RenderComponent* render = GameState::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    return entity;
};

EntityHandle GameFactory::CreateHealthPickup(const glm::vec2& position, int heal)
{
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(40.0f, 40.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = GameState::CollisionComponents().Create(entity);
    collision->SetBoundingBox(glm::vec4(-20.0f, -20.0f, 20.0f, 20.0f));
    collision->SetType(CollisionTypes::Pickup);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(glm::vec2(-100.0f, 0.0f)));
    script->AddScript(std::make_shared<HealthPickupScript>(20));

    RenderComponent* render = GameState::RenderComponents().Create(entity);
    render->SetDiffuseColor(glm::vec4(0.6f, 1.0f, 0.6f, 1.0f));

    return entity;
}

EntityHandle GameFactory::CreateSpawner(const glm::vec2& position)
{
    EntityHandle entity = GameState::EntitySystem().CreateEntity();

    TransformComponent* transform = GameState::TransformComponents().Create(entity);
    transform->SetPosition(position);

    ScriptComponent* script = GameState::ScriptComponents().Create(entity);
    script->AddScript(std::make_shared<SpawnerScript>());

    return entity;
}
