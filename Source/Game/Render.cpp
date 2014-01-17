#include "Precompiled.hpp"
#include "Render.hpp"

Render::Render() :
    m_color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

Render::~Render()
{
}

void Render::SetColor(const glm::vec4& color)
{
    m_color = color;
}

const glm::vec4& Render::GetColor() const
{
    return m_color;
}
