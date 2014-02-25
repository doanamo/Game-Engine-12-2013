#include "Precompiled.hpp"
#include "CommonScripts.hpp"

#include "GameContext.hpp"
#include "RenderComponent.hpp"

BlinkOnDamageScript::BlinkOnDamageScript() :
    m_render(nullptr),
    m_timer(0.0f)
{
}

void BlinkOnDamageScript::OnUpdate(EntityHandle self, float timeDelta)
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
    
void BlinkOnDamageScript::OnDamage(EntityHandle self, int value, bool alive)
{
    // Get the needed render component.
    RenderComponent* render = GetRenderComponent(self);
    if(render == nullptr) return;

    // Set the initial blink.
    m_timer = 0.8f;

    render->SetEmissionColor(glm::vec3(1.0f, 1.0f, 1.0f));
    render->SetEmissionPower(m_timer);
}

RenderComponent* BlinkOnDamageScript::GetRenderComponent(EntityHandle self)
{
    // Cache the render component.
    if(m_render == nullptr)
    {
        m_render = Game::RenderComponents().Lookup(self);
    }

    return m_render;
}
