#pragma once

#include "Precompiled.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/TextDrawState.hpp"

// Forward declarations.
class Font;

//
// Text Draw Align
//

struct TextDrawAlign
{
    enum Type
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Centered,

        Default = TopLeft,
    };
};

//
// Text Draw Info
//

struct TextDrawInfo
{
    // Default constructor.
    TextDrawInfo() :
        font(nullptr),
        size(12),
        align(TextDrawAlign::Default),
        bodyColor(0.0f, 0.0f, 0.0f, 1.0f),
        outlineColor(1.0f, 1.0f, 1.0f, 0.0f),
        glowColor(0.0f, 0.0f, 0.0f, 0.0f),
        outlineRange(0.5f, 0.5f),
        glowRange(0.3f, 0.6f),
        position(0.0f, 0.0f),
        area(0.0f, 0.0f),
        cursorIndex(-1),
        debug(false)
    {
    }

    // Font used.
    Font* font;

    // Font size;
    float size;

    // Text align.
    TextDrawAlign::Type align;

    // Color of the text body.
    glm::vec4 bodyColor;

    // Color of the text outline.
    glm::vec4 outlineColor;

    // Color of the text glow.
    glm::vec4 glowColor;

    // Outline range (0.0f - 1.0f).
    glm::vec2 outlineRange;

    // Glow range (0.0f - 1.0f).
    glm::vec2 glowRange;

    // Top left corner of the text bounding box (not base line).
    glm::vec2 position;

    // Size of the drawing area that text will be enclosed in. 
    // If a word exceeds the width, it will be wrote in a new line.
    // If a line exceeds the height, it will be cut out (possibly partially).
    // Set width or height to 0.0f to disable each.
    glm::vec2 area;

    // Text cursor index (position).
    // Set to -1 to disable.
    int cursorIndex;

    // Draws debug info.
    bool debug;
};

//
// Text Draw Metrics
//

struct TextDrawMetrics
{
    // Default constructor.
    TextDrawMetrics() :
        textArea(0.0f, 0.0f, 0.0f, 0.0f),
        boundingBox(0.0f, 0.0f, 0.0f, 0.0f),
        lines(0)
    {
    }

    // Text drawing area.
    glm::vec4 textArea;

    // Text bounding box.
    glm::vec4 boundingBox;

    // Text lines.
    int lines;
};

//
// Text Renderer
//

class TextRenderer
{
public:
    TextRenderer();
    ~TextRenderer();

    bool Initialize(int bufferSize = 128);
    void Cleanup();

    void UpdateCursorBlink(float dt);
    void ResetCursorBlink();

    TextDrawMetrics Measure(const TextDrawInfo& info, const char* text);
    void Draw(const TextDrawInfo& info, const char* text, const glm::mat4& transform);

private:
    // Text draw state.
    TextDrawState m_drawState;

    // Batched glyphs.
    struct GlyphData
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 scale;
        glm::vec2 texture; // Texture origin in pixels.
        glm::vec4 bodyColor;
        glm::vec4 outlineColor;
        glm::vec4 glowColor;
        glm::vec2 outlineRange;
        glm::vec2 glowRange;
    };

    GlyphData* m_bufferData;
    int        m_bufferSize;

    // Graphics objects.
    Shader         m_shader;
    VertexBuffer   m_vertexBuffer;
    InstanceBuffer m_instanceBuffer;
    VertexInput    m_vertexInput;

    // Current text cursor blink time.
    float m_cursorBlinkTime;

    bool m_initialized;
};
