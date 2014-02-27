#pragma once

#include "Precompiled.hpp"

//
// Health Bar
//

class HealthBar
{
public:
    HealthBar();
    ~HealthBar();

    void SetDrawingRectangle(const glm::vec4& rectangle);

    void SetCurrentValue(float value);
    void SetMaximumValue(float value);
    void SetDecayingSpeed(float speed);

    void Update(float timeDelta);
    void Draw(const glm::vec2& position, const glm::mat4& transform);

private:
    glm::vec4 m_drawingRectangle;

    float m_currentValue;
    float m_maximumValue;
    float m_decayingValue;
    float m_decayingSpeed;
};
