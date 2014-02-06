#include "Precompiled.hpp"
#include "TransformComponent.hpp"

TransformComponent::TransformComponent() :
    m_position(0.0f, 0.0f),
    m_scale(1.0f, 1.0f),
    m_rotation(0.0f)
{
}

TransformComponent::~TransformComponent()
{
}

glm::mat4 TransformComponent::CalculateMatrix(const glm::mat4& base)
{
    glm::mat4 output;
    output = glm::translate(base, glm::vec3(m_position, 0.0f));
    output = glm::rotate(output, m_rotation, glm::vec3(0.0f, 0.0f, -1.0f));
    output = glm::scale(output, glm::vec3(m_scale, 1.0f));
    return output;
}

glm::vec2 TransformComponent::CalculateDirection()
{
    glm::vec2 output(0.0f);
    output.x = glm::sin(glm::radians(m_rotation));
    output.y = glm::cos(glm::radians(m_rotation));
    return output;
}
