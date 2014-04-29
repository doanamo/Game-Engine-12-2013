#include "Precompiled.hpp"
#include "CommonScripts.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Transform/TransformComponent.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Health/HealthSystem.hpp"
#include "Game/Render/RenderComponent.hpp"

//
// Constant Velocity
//

ConstantVelocityScript::ConstantVelocityScript(const glm::vec2& velocity) :
    m_transform(nullptr),
    m_velocity(velocity)
{
}

void ConstantVelocityScript::OnUpdate(EntityHandle self, float timeDelta)
{
    // Get the needed transform component.
    TransformComponent* transform = GetTransformComponent(self);
    if(transform == nullptr) return;

    // Apply the constant velocity.
    glm::vec2 position = transform->GetPosition();
    position += m_velocity * timeDelta;
    transform->SetPosition(position);
}

TransformComponent* ConstantVelocityScript::GetTransformComponent(EntityHandle self)
{
    // Cache the transform component.
    if(m_transform == nullptr)
    {
        m_transform = GameState::TransformComponents().Lookup(self);
    }

    return m_transform;
}

//
// Damage On Collision
//

DamageOnCollision::DamageOnCollision(int damage, float interval) :
    m_damage(damage),
    m_interval(interval)
{
}

void DamageOnCollision::OnCollision(const CollisionObject& self, const CollisionObject& other)
{
    assert(self.collision != nullptr);
    assert(other.collision != nullptr);

    // Apply damage to target entity.
    GameState::HealthSystem().Damage(other.entity, m_damage);

    // Temporarily disable collision response with this entity.
    GameState::CollisionSystem().DisableCollisionResponse(self.entity, other.entity, m_interval);
}

//
// Destroy On Collision
//

void DestroyOnCollision::OnCollision(const CollisionObject& self, const CollisionObject& other)
{
    assert(other.collision != nullptr);

    // Destroy entity.
    GameState::EntitySystem().DestroyEntity(other.entity);
}

//
// Destroy On Death
//

void DestroyOnDeathScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        GameState::EntitySystem().DestroyEntity(self);
    }
}

//
// Flash On Damage
//

FlashOnDamageScript::FlashOnDamageScript() :
    m_render(nullptr),
    m_timer(0.0f)
{
}

void FlashOnDamageScript::OnUpdate(EntityHandle self, float timeDelta)
{
    if(m_timer == 0.0f)
        return;

    // Get the needed render component.
    RenderComponent* render = GetRenderComponent(self);
    if(render == nullptr) return;

    // Update the blink.
    m_timer = std::max(0.0f, m_timer - timeDelta);

    render->SetEmissionColor(glm::vec3(1.0f, 1.0f, 1.0f));
    render->SetEmissionPower(m_timer);
}
    
void FlashOnDamageScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Get the needed render component.
    RenderComponent* render = GetRenderComponent(self);
    if(render == nullptr) return;

    // Set the initial blink.
    m_timer = 0.8f;

    render->SetEmissionColor(glm::vec3(1.0f, 1.0f, 1.0f));
    render->SetEmissionPower(m_timer);
}

RenderComponent* FlashOnDamageScript::GetRenderComponent(EntityHandle self)
{
    // Cache the render component.
    if(m_render == nullptr)
    {
        m_render = GameState::RenderComponents().Lookup(self);
    }

    return m_render;
}
