#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Transform Component
//

class TransformComponent : public Component
{
public:
    TransformComponent();
    ~TransformComponent();

    // Calculates the transform matrix.
    glm::mat4 CalculateMatrix(const glm::mat4& base = glm::mat4(1.0f));

    // Calculates the facing direction.
    glm::vec2 CalculateDirection();

    // Sets the position.
    void SetPosition(const glm::vec2& position)
    {
        m_position = position;
    }

    // Sets the scale.
    void SetScale(const glm::vec2& scale)
    {
        m_scale = scale;
    }

    // Sets the rotation.
    void SetRotation(float rotation)
    {
        m_rotation = glm::mod(rotation, 360.0f);
    }

    // Gets the position.
    const glm::vec2& GetPosition() const
    {
        return m_position;
    }

    // Gets the scale.
    const glm::vec2& GetScale() const
    {
        return m_scale;
    }

    // Gets the rotation.
    float GetRotation() const
    {
        return m_rotation;
    }

private:
    // Transform data.
    glm::vec2 m_position;
    glm::vec2 m_scale;
    float m_rotation;
};
