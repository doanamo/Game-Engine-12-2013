#pragma once

#include "Precompiled.hpp"
#include "Component.hpp"

//
// Transform Component
//

class Transform : public Component
{
public:
    Transform();
    ~Transform();

    // Calculates the transform matrix.
    glm::mat4 CalculateMatrix(const glm::mat4& base = glm::mat4(1.0f));

    // Calculates the facing direction.
    glm::vec2 CalculateDirection();

    // Sets the position.
    void SetPosition(const glm::vec2& position);

    // Sets the scale.
    void SetScale(const glm::vec2& scale);

    // Sets the rotation.
    void SetRotation(float rotation);

    // Gets the position.
    const glm::vec2& GetPosition() const;

    // Gets the scale.
    const glm::vec2& GetScale() const;

    // Gets the rotation.
    float GetRotation() const;

private:
    // Transform data.
    glm::vec2 m_position;
    glm::vec2 m_scale;
    float m_rotation;
};
