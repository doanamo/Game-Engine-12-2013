#include "Precompiled.hpp"
#include "ValueBar.hpp"

#include "MainContext.hpp"
#include "Graphics/ShapeRenderer.hpp"

ValueBar::ValueBar() :
    m_drawingRectangle(0.0f, 0.0f, 100.0f, 10.0f),
    m_foregroundColor(1.0f, 0.0f, 0.0f, 1.0f),
    m_backgroundColor(0.0f, 0.0f, 0.0f, 1.0f),
    m_decayColor(1.0f, 1.0f, 0.0f, 1.0f),
    m_currentValue(100.0f),
    m_maximumValue(100.0f),
    m_decayingValue(m_currentValue),
    m_decayingSpeed(10.0f)
{
}

ValueBar::~ValueBar()
{
    Cleanup();
}

void ValueBar::Cleanup()
{
    m_drawingRectangle = glm::vec4(0.0f, 0.0f, 100.0f, 10.0f);
    m_foregroundColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    m_backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    m_decayColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    m_currentValue = 100.0f;
    m_maximumValue = 100.0f;
    m_decayingValue = m_currentValue;
    m_decayingSpeed = 10.0f;
}

void ValueBar::SetDrawingRectangle(const glm::vec4& rectangle)
{
    m_drawingRectangle = rectangle;
}

void ValueBar::SetForegroundColor(const glm::vec4&color)
{
    m_foregroundColor = color;
}

void ValueBar::SetBackgroundColor(const glm::vec4&color)
{
    m_backgroundColor = color;
}

void ValueBar::SetDecayColor(const glm::vec4&color)
{
    m_decayColor = color;
}

void ValueBar::SetCurrentValue(float value)
{
    m_currentValue = std::min(value, m_maximumValue);

    if(m_currentValue > m_decayingValue)
    {
        m_decayingValue = m_currentValue;
    }
}

void ValueBar::SetMaximumValue(float value)
{
    m_maximumValue = value;
}

void ValueBar::SetDecayingSpeed(float speed)
{
    m_decayingSpeed = speed;
}

void ValueBar::Update(float timeDelta)
{
    if(m_decayingValue > m_currentValue)
    {
        m_decayingValue = std::max(m_currentValue, m_decayingValue - m_decayingSpeed * timeDelta);
    }
}

void ValueBar::Draw(const glm::vec2& position, const glm::mat4& transform)
{
    ShapeRenderer::Quad quads[3];

    // Define the background quad.
    quads[0].color = m_backgroundColor;
    quads[0].position = glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y) + position;
    quads[0].size = glm::vec2(m_drawingRectangle.z, m_drawingRectangle.w);
    quads[0].texture = nullptr;

    // Define the decaying value quad.
    quads[1].color = m_decayColor;
    quads[1].position = glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y) + position;
    quads[1].size = glm::vec2(m_drawingRectangle.z, m_drawingRectangle.w);
    quads[1].texture = nullptr;

    // Define the current value quad.
    quads[2].color = m_foregroundColor;
    quads[2].position = glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y) + position;
    quads[2].size = glm::vec2(m_drawingRectangle.z, m_drawingRectangle.w);
    quads[2].texture = nullptr;

    // Scale quads for the current values.
    quads[1].size.x = quads[1].size.x / m_maximumValue * m_decayingValue;
    quads[2].size.x = quads[2].size.x / m_maximumValue * m_currentValue;

    // Draw the health bar.
    Main::ShapeRenderer().DrawQuads(&quads[0], StaticArraySize(quads), transform);
}
