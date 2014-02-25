#include "Precompiled.hpp"
#include "RenderComponent.hpp"

RenderComponent::RenderComponent() :
    m_diffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissionColor(1.0f, 1.0f, 1.0f),
    m_emissionPower(0.0f)
{
}

RenderComponent::~RenderComponent()
{
}
