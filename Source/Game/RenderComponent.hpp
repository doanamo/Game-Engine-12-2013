#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Render Component
//

class RenderComponent : public Component
{
public:
    RenderComponent();
    ~RenderComponent();

    void SetDiffuseColor(const glm::vec4& color)
    {
        m_diffuseColor = color;
    }

    void SetEmissionColor(const glm::vec3& color)
    {
        m_emissionColor = color;
    }

    void SetEmissionPower(float power)
    {
        m_emissionPower = power;
    }

    const glm::vec4& GetDiffuseColor() const
    {
        return m_diffuseColor;
    }

    const glm::vec3& GetEmissionColor() const
    {
        return m_emissionColor;
    }

    float GetEmissionPower() const
    {
        return m_emissionPower;
    }

private:
    // Render properties.
    glm::vec4 m_diffuseColor;
    glm::vec3 m_emissionColor;
    float     m_emissionPower;
};
