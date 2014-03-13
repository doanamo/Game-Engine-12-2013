#pragma once

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

// Forward declarations.
class Font;

//
// Text Renderer
//

class TextRenderer
{
public:
    // Draw description.
    struct DrawInfo
    {
        // Default constructor.
        DrawInfo() :
            font(nullptr),
            color(0.0f, 0.0f, 0.0f, 1.0f),
            position(0.0f, 0.0f),
            size(0.0f, 0.0f),
            cursorIndex(-1),
            debug(false)
        {
        }

        // Font used.
        Font* font;

        // Color of the drawn text.
        glm::vec4 color;

        // Top left corner of the text bounding box (not base line).
        glm::vec2 position;

        // Size of the drawing area that text will be enclosed in. 
        // If a word exceeds the width, it will be wrote in a new line.
        // If a line exceeds the height, it will be cut out (possibly partially).
        // Set width or height to 0.0f to disable each.
        glm::vec2 size;

        // Text cursor index (position).
        // Set to -1 to disable.
        int cursorIndex;

        // Draws debug info.
        bool debug;
    };

    // Draw metrics.
    struct DrawMetrics
    {
        // Default constructor.
        DrawMetrics() :
            boundingBox(0.0f, 0.0f, 0.0f, 0.0f),
            drawingArea(0.0f, 0.0f, 0.0f, 0.0f),
            lines(0)
        {
        }

        // Text bounding box.
        glm::vec4 boundingBox;

        // Text drawing area.
        glm::vec4 drawingArea;

        // Text lines.
        int lines;
    };

public:
    TextRenderer();
    ~TextRenderer();

    bool Initialize(int bufferSize);
    void Cleanup();

    void UpdateCursorBlink(float dt);
    void ResetCursorBlink();

    DrawMetrics Measure(const DrawInfo& info, const char* text);
    void Draw(const DrawInfo& info, const glm::mat4& transform, const char* text);

private:
    // Batched glyphs.
    struct GlyphData
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 scale;
        glm::vec2 texture; // Texture origin in pixels.
        glm::vec4 color;
    };

    GlyphData* m_bufferData;
    int        m_bufferSize;

    // Render objects.
    Shader         m_shader;
    VertexBuffer   m_vertexBuffer;
    InstanceBuffer m_instanceBuffer;
    VertexInput    m_vertexInput;

    // Current text cursor blink.
    float m_cursorBlinkTime;

    bool m_initialized;
};
