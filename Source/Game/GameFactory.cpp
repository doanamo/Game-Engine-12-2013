#include "Precompiled.hpp"
#include "GameFactory.hpp"

#include "Game/GameGlobal.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Component/ComponentSystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputComponent.hpp"
#include "Game/Input/InputSystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionObject.hpp"
#include "Game/Collision/CollisionComponent.hpp"
#include "Game/Collision/CollisionDefinitions.hpp"
#include "Game/Health/HealthComponent.hpp"
#include "Game/Script/ScriptComponent.hpp"
#include "Game/Script/CommonScripts.hpp"
#include "Game/Script/EnemyScript.hpp"
#include "Game/Script/ProjectileScript.hpp"
#include "Game/Script/HealthPickupScript.hpp"
#include "Game/Script/SpawnerScript.hpp"
#include "Game/Render/RenderComponent.hpp"

namespace
{
    std::random_device deviceRandom;
}

namespace
{
    std::mt19937 enemyRandom(deviceRandom());
}

EntityHandle GameFactory::CreateEnemy(const glm::vec2& position)
{
    EntityHandle entity = GameState::GetEntitySystem().CreateEntity();

    TransformComponent* transform = GameState::GetComponentSystem().Create<TransformComponent>(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(50.0f, 50.0f));
    transform->SetRotation(0.0f);

    HealthComponent* health = GameState::GetComponentSystem().Create<HealthComponent>(entity);
    health->SetMaximumHealth(30);
    health->SetCurrentHealth(30);

    CollisionComponent* collision = GameState::GetComponentSystem().Create<CollisionComponent>(entity);
    collision->SetBoundingBox(glm::vec4(-25.0f, -25.0f, 25.0f, 25.0f));
    collision->SetType(CollisionTypes::Enemy);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = GameState::GetComponentSystem().Create<ScriptComponent>(entity);
    script->AddScript(std::make_shared<EnemyScript>());
    script->AddScript(std::make_shared<DamageOnCollision>(5, 0.2f));
    script->AddScript(std::make_shared<FlashOnDamageScript>());

    RenderComponent* render = GameState::GetComponentSystem().Create<RenderComponent>(entity);
    render->SetDiffuseColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    return entity;
}

EntityHandle GameFactory::CreateProjectile(const glm::vec2& position, const glm::vec2& direction, float speed, uint32_t collisionMask)
{
    EntityHandle entity = GameState::GetEntitySystem().CreateEntity();

    TransformComponent* transform = GameState::GetComponentSystem().Create<TransformComponent>(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(30.0f, 30.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = GameState::GetComponentSystem().Create<CollisionComponent>(entity);
    collision->SetBoundingBox(glm::vec4(-15.0f, -15.0f, 15.0f, 15.0f));
    collision->SetType(CollisionTypes::Projectile);
    collision->SetMask(collisionMask);

    ScriptComponent* script = GameState::GetComponentSystem().Create<ScriptComponent>(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(direction * speed));
    script->AddScript(std::make_shared<ProjectileScript>());

    RenderComponent* render = GameState::GetComponentSystem().Create<RenderComponent>(entity);
    render->SetDiffuseColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    return entity;
};

EntityHandle GameFactory::CreateHealthPickup(const glm::vec2& position, int heal)
{
    EntityHandle entity = GameState::GetEntitySystem().CreateEntity();

    TransformComponent* transform = GameState::GetComponentSystem().Create<TransformComponent>(entity);
    transform->SetPosition(position);
    transform->SetScale(glm::vec2(40.0f, 40.0f));
    transform->SetRotation(0.0f);

    CollisionComponent* collision = GameState::GetComponentSystem().Create<CollisionComponent>(entity);
    collision->SetBoundingBox(glm::vec4(-20.0f, -20.0f, 20.0f, 20.0f));
    collision->SetType(CollisionTypes::Pickup);
    collision->SetMask(CollisionTypes::Player);

    ScriptComponent* script = GameState::GetComponentSystem().Create<ScriptComponent>(entity);
    script->AddScript(std::make_shared<ConstantVelocityScript>(glm::vec2(-100.0f, 0.0f)));
    script->AddScript(std::make_shared<HealthPickupScript>(20));

    RenderComponent* render = GameState::GetComponentSystem().Create<RenderComponent>(entity);
    render->SetDiffuseColor(glm::vec4(0.6f, 1.0f, 0.6f, 1.0f));

    return entity;
}

EntityHandle GameFactory::CreateSpawner(const glm::vec2& position)
{
    EntityHandle entity = GameState::GetEntitySystem().CreateEntity();

    TransformComponent* transform = GameState::GetComponentSystem().Create<TransformComponent>(entity);
    transform->SetPosition(position);

    ScriptComponent* script = GameState::GetComponentSystem().Create<ScriptComponent>(entity);
    script->AddScript(std::make_shared<SpawnerScript>());

    return entity;
}
