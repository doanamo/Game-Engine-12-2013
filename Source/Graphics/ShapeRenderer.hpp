#pragma once

#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

// Forward declarations.
class Texture;

//
// Shape Renderer
//

class ShapeRenderer
{
private:
    // Vertex structure.
    struct Vertex
    {
        glm::vec2 position;
        glm::vec2 texture;
        glm::vec4 color;
    };

public:
    // Line shape structure.
    struct Line
    {
        glm::vec2 begin;
        glm::vec2 end;
        glm::vec4 color;
    };

    // Rectangle shape structure.
    struct Rectangle
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec4 color;
    };

    // Quad shape structure.
    struct Quad
    {
        Quad() :
            texture(nullptr)
        {
        }

        glm::vec2 position;
        glm::vec2 size;
        glm::vec4 color;

        const Texture* texture;
    };

public:
    ShapeRenderer();
    ~ShapeRenderer();

    bool Initialize(int bufferSize);
    void Cleanup();

    void DrawLines(const Line* data, int count, const glm::mat4& transform);
    void DrawRectangles(const Rectangle* data, int count, const glm::mat4& transform);
    void DrawQuads(const Quad* data, int count, const glm::mat4& transform);

private:
    Vertex* m_bufferData;
    int     m_bufferSize;

    Shader       m_shader;
    VertexBuffer m_vertexBuffer;
    VertexInput  m_vertexInput;

    bool m_initialized;
};
