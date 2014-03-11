#include "Precompiled.hpp"
#include "CommonScripts.hpp"

#include "GameContext.hpp"

#include "EntitySystem.hpp"

#include "TransformComponent.hpp"
#include "RenderComponent.hpp"

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
        m_transform = Game::TransformComponents().Lookup(self);
    }

    return m_transform;
}

//
// Destroy On Death
//

void DestroyOnDeathScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Destroy the entity on fatal damage.
    if(!alive)
    {
        Game::EntitySystem().DestroyEntity(self);
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
        m_render = Game::RenderComponents().Lookup(self);
    }

    return m_render;
}
