#pragma once

#include "Precompiled.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

// Forward declarations.
class Texture;

//
// Basic Renderer
//

class BasicRenderer
{
public:
    // Line types.
    enum class LineType
    {
        Rough,    // Default line rasterization.
        Adjusted, // Adjusted to the center of the pixel.
        Smooth,   // Constructed from textures quads.
    };

    // Draw modes.
    enum class DrawMode
    {
        Line,
        Fill,
    };

    // Line shape.
    struct Line
    {
        glm::vec4 color;
        glm::vec2 begin;
        glm::vec2 end;
    };

    struct LineStyle
    {
        LineStyle() :
            lineType(LineType::Rough),
            alphaBlend(false)
        {
        }

        LineType lineType;
        bool alphaBlend;
    };

    // Rectangle shape.
    struct Rectangle
    {
        glm::vec4 color;
        glm::vec2 bottomleft;
        glm::vec2 topright;
    };

    struct RectangleStyle
    {
        RectangleStyle() :
            drawMode(DrawMode::Fill),
            lineType(LineType::Rough),
            alphaBlend(false)
        {
        }

        DrawMode drawMode;
        LineType lineType;
        bool alphaBlend;
    };

    // Circle shape.
    struct Circle
    {
        Circle() :
            radius(1.0f)
        {
        }

        glm::vec4 color;
        glm::vec2 position;
        float radius;
    };

    struct CircleStyle
    {
        CircleStyle() :
            drawMode(DrawMode::Fill),
            lineType(LineType::Rough),
            alphaBlend(false)
        {
        }

        DrawMode drawMode;
        LineType lineType;
        bool alphaBlend;
    };

    // Sprite object.
    struct Sprite
    {
        Sprite() :
            texture(nullptr)
        {
        }

        glm::vec2 position;
        glm::vec2 size;
        glm::vec4 color;

        const Texture* texture;
    };

    struct SpriteStyle
    {
        SpriteStyle() :
            alphaBlend(false)
        {
        }

        bool alphaBlend;
    };

public:
    BasicRenderer();
    ~BasicRenderer();

    bool Initialize(int bufferSize = 128);
    void Cleanup();

    void DrawLines(const LineStyle& style, const Line* data, int count, const glm::mat4& transform);
    void DrawRectangles(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform);
    void DrawCircles(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform);
    void DrawSprites(const SpriteStyle& style, const Sprite* data, int count, const glm::mat4& transform);

private:
    void DrawLinesRough(const LineStyle& style, const Line* data, int count, const glm::mat4& transform);
    void DrawLinesSmooth(const LineStyle& style, const Line* data, int count, const glm::mat4& transform);
    void DrawRectanglesLine(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform);
    void DrawRectanglesFill(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform);
    void DrawCirclesLine(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform);
    void DrawCirclesFill(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform);

private:
    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
        glm::vec4 color;
    };

private:
    // Buffer data.
    Vertex* m_bufferData;
    int     m_bufferSize;

    // Graphics objects.
    Shader       m_shader;
    VertexBuffer m_vertexBuffer;
    VertexInput  m_vertexInput;

    bool m_initialized;
};
