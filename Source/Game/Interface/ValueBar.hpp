#pragma once

#include "Precompiled.hpp"

//
// Value Bar
//

class ValueBar
{
public:
    ValueBar();
    ~ValueBar();

    void Cleanup();

    void SetDrawingRectangle(const glm::vec4& rectangle);
    void SetForegroundColor(const glm::vec4&color);
    void SetBackgroundColor(const glm::vec4&color);
    void SetDecayColor(const glm::vec4&color);

    void SetCurrentValue(float value);
    void SetMaximumValue(float value);
    void SetDecayingSpeed(float speed);

    void Update(float timeDelta);
    void Draw(const glm::vec2& position, const glm::mat4& transform);

private:
    glm::vec4 m_drawingRectangle;
    glm::vec4 m_foregroundColor;
    glm::vec4 m_backgroundColor;
    glm::vec4 m_decayColor;

    float m_currentValue;
    float m_maximumValue;
    float m_decayingValue;
    float m_decayingSpeed;
};
