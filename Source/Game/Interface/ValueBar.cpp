#include "Precompiled.hpp"
#include "ValueBar.hpp"

#include "MainContext.hpp"
#include "Graphics/BasicRenderer.hpp"

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
    BasicRenderer::Rectangle rectangles[3];

    // Scaling values for rectangles.
    float decayRectangleWidth = m_drawingRectangle.z / m_maximumValue * m_decayingValue;
    float currentRectangleWidth = m_drawingRectangle.z / m_maximumValue * m_currentValue;

    // Define the background rectangle.
    rectangles[0].color = m_backgroundColor;
    rectangles[0].bottomleft = position + glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y);
    rectangles[0].topright = position + glm::vec2(m_drawingRectangle.z, m_drawingRectangle.w);

    // Define the decaying value rectangle.
    rectangles[1].color = m_decayColor;
    rectangles[1].bottomleft = position + glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y);
    rectangles[1].topright = position + glm::vec2(decayRectangleWidth, m_drawingRectangle.w);

    // Define the current value rectangle.
    rectangles[2].color = m_foregroundColor;
    rectangles[2].bottomleft = position + glm::vec2(m_drawingRectangle.x, m_drawingRectangle.y);
    rectangles[2].topright = position + glm::vec2(currentRectangleWidth, m_drawingRectangle.w);

    // Draw the health bar.
    BasicRenderer::RectangleStyle style;
    style.drawMode = BasicRenderer::DrawMode::Fill;
    style.alphaBlend = false;

    Main::BasicRenderer().DrawRectangles(style, &rectangles[0], StaticArraySize(rectangles), transform);
}
