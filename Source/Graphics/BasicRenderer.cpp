#include "Precompiled.hpp"
#include "BasicRenderer.hpp"

#include "MainContext.hpp"
#include "Graphics/Texture.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a shape renderer! "

    // Minimum buffer size allowed.
    const int MinimumBufferSize = 8;
}

BasicRenderer::BasicRenderer() :
    m_bufferData(nullptr),
    m_bufferSize(0),
    m_initialized(false)
{
}

BasicRenderer::~BasicRenderer()
{
    Cleanup();
}

void BasicRenderer::Cleanup()
{
    delete[] m_bufferData;
    m_bufferData = nullptr;

    m_bufferSize = 0;

    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_initialized = false;
}

bool BasicRenderer::Initialize(int bufferSize)
{
    Cleanup();
    
    // Validate arguments.
    if(bufferSize <= MinimumBufferSize)
    {
        Log() << LogInitializeError() << "Invalid argument - \"bufferSize\" is invalid.";
        return false;
    }

    m_bufferSize = bufferSize;

    // Allocate buffer memory.
    m_bufferData = new Vertex[m_bufferSize];

    // Load a shader.
    if(!m_shader.Load("Data/Shaders/Basic.glsl"))
    {
        Log() << LogInitializeError() << "Failed to load a shader.";
        Cleanup();
        return false;
    }

    // Create a vertex buffer.
    if(!m_vertexBuffer.Initialize(sizeof(Vertex), m_bufferSize, nullptr, GL_DYNAMIC_DRAW))
    {
        Log() << LogInitializeError() << "Failed to create a vertex buffer.";
        Cleanup();
        return false;
    }

    // Create a vertex input.
    VertexAttribute vertexAttributes[3] =
    {
        { &m_vertexBuffer, VertexAttributeTypes::Float2 }, // Position
        { &m_vertexBuffer, VertexAttributeTypes::Float2 }, // Texture
        { &m_vertexBuffer, VertexAttributeTypes::Float4 }, // Color
    };

    if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
    {
        Log() << LogInitializeError() << "Failed to create a vertex input.";
        Cleanup();
        return false;
    }

    // Return success.
    m_initialized = true;

    return true;
}

void BasicRenderer::DrawLines(const LineStyle& style, const Line* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    if(data == nullptr)
        return;

    if(count <= 0)
        return;

    // Draw based on style.
    if(style.lineType == LineType::Rough || style.lineType == LineType::Adjusted)
    {
        DrawLinesRough(style, data, count, transform);
    }
    else
    if(style.lineType == LineType::Smooth)
    {
        DrawLinesSmooth(style, data, count, transform);
    }
}

void BasicRenderer::DrawLinesRough(const LineStyle& style, const Line* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    assert(data != nullptr);
    assert(count > 0);

    // Minimum buffer size needed.
    const int ShapeVerticeCount = 2;
    assert(m_bufferSize >= ShapeVerticeCount);

    // Bind rendering states.
    if(style.alphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    SCOPE_GUARD(glDisable(GL_BLEND));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Main::GetBlankTexture().GetHandle());
    SCOPE_GUARD(glBindTexture(GL_TEXTURE_2D, 0));

    glBindVertexArray(m_vertexInput.GetHandle());
    SCOPE_GUARD(glBindVertexArray(0));

    glUseProgram(m_shader.GetHandle());
    SCOPE_GUARD(glUseProgram(0));

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    // Line rasterization workaround. This will make line vertex positions pixel perfect.
    // Transform must be in window space for this to work. Could cause problems with antialiasing.
    // See: http://stackoverflow.com/questions/10040961/opengl-pixel-perfect-2d-drawing
    glm::vec2 offset(0.0f, 0.0f);

    if(style.lineType == LineType::Adjusted)
    {
        offset = glm::vec2(0.5f, 0.5f);
    }

    // Batch shapes and draw them.
    int verticesBatched = 0;

    for(int i = 0; i < count; ++i)
    {
        const Line& line = data[i];

        // Create shapes vertices.
        Vertex shapeVertices[ShapeVerticeCount] =
        {
            { line.begin + offset, glm::vec2(0.0f, 0.0f), line.color },
            { line.end   + offset, glm::vec2(0.0f, 0.0f), line.color },
        };

        // Copy vertices to a temporary buffer.
        memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

        verticesBatched += ShapeVerticeCount;

        // Determine if we have to draw the batch.
        bool drawBatch = false;

        if(m_bufferSize - verticesBatched < ShapeVerticeCount)
        {
            // We reached the buffer size.
            drawBatch = true;
        }
        else
        if(i == count - 1)
        {
            // We reached the last element.
            drawBatch = true;
        }

        // Draw the batch if needed.
        if(drawBatch)
        {
            // Update vertex buffer.
            m_vertexBuffer.Update(m_bufferData);

            // Draw shapes.
            glDrawArrays(GL_LINES, 0, verticesBatched);

            // Reset batch counter.
            verticesBatched = 0;
        }
    }
}

void BasicRenderer::DrawLinesSmooth(const LineStyle& style, const Line* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Not implemented yet.
    // Could utilize geometry shader and only have one
    // function for drawing both rough and smooth lines.
}

void BasicRenderer::DrawRectangles(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    if(data == nullptr)
        return;

    if(count <= 0)
        return;

    // Draw based on mode.
    if(style.drawMode == DrawMode::Line)
    {
        DrawRectanglesLine(style, data, count, transform);
    }
    else
    if(style.drawMode == DrawMode::Fill)
    {
        DrawRectanglesFill(style, data, count, transform);
    }
}

void BasicRenderer::DrawRectanglesLine(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    assert(data != nullptr);
    assert(count > 0);

    // Minimum buffer size needed.
    const int ShapeVerticeCount = 8;
    assert(m_bufferSize >= ShapeVerticeCount);

    // Bind rendering states.
    if(style.alphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    SCOPE_GUARD(glDisable(GL_BLEND));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Main::GetBlankTexture().GetHandle());
    SCOPE_GUARD(glBindTexture(GL_TEXTURE_2D, 0));

    glBindVertexArray(m_vertexInput.GetHandle());
    SCOPE_GUARD(glBindVertexArray(0));

    glUseProgram(m_shader.GetHandle());
    SCOPE_GUARD(glUseProgram(0));

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    // Line rasterization workaround. This will make line vertex positions pixel perfect.
    // Transform must be in window space for this to work. Could cause problems with antialiasing.
    // See: http://stackoverflow.com/questions/10040961/opengl-pixel-perfect-2d-drawing
    glm::vec2 offset(0.0f, 0.0f);

    if(style.lineType == LineType::Adjusted)
    {
        offset = glm::vec2(0.5f, 0.5f);
    }

    // Batch shapes and draw them.
    int verticesBatched = 0;

    for(int i = 0; i < count; ++i)
    {
        const Rectangle& rectangle = data[i];

        // Create vertices.
        glm::vec4 rect(rectangle.bottomleft, rectangle.topright);

        Vertex shapeVertices[ShapeVerticeCount] =
        {
            { glm::vec2(rect.x, rect.y) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.z, rect.y) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },

            { glm::vec2(rect.z, rect.y) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.z, rect.w) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },

            { glm::vec2(rect.z, rect.w) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.x, rect.w) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },

            { glm::vec2(rect.x, rect.w) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.x, rect.y) + offset, glm::vec2(0.0f, 0.0f), rectangle.color },
        };

        // Copy vertices to a temporary buffer.
        memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

        verticesBatched += ShapeVerticeCount;

        // Determine if we have to draw the batch.
        bool drawBatch = false;

        if(m_bufferSize - verticesBatched < ShapeVerticeCount)
        {
            // We reached the buffer size.
            drawBatch = true;
        }
        else
        if(i == count - 1)
        {
            // We reached the last element.
            drawBatch = true;
        }

        // Draw the batch if needed.
        if(drawBatch)
        {
            // Update vertex buffer.
            m_vertexBuffer.Update(m_bufferData);

            // Draw shapes.
            glDrawArrays(GL_LINES, 0, verticesBatched);

            // Reset batch counter.
            verticesBatched = 0;
        }
    }
}

void BasicRenderer::DrawRectanglesFill(const RectangleStyle& style, const Rectangle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    assert(data != nullptr);
    assert(count > 0);

    // Minimum buffer size needed.
    const int ShapeVerticeCount = 6;
    assert(m_bufferSize >= ShapeVerticeCount);

    // Bind rendering states.
    if(style.alphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    SCOPE_GUARD(glDisable(GL_BLEND));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Main::GetBlankTexture().GetHandle());
    SCOPE_GUARD(glBindTexture(GL_TEXTURE_2D, 0));

    glBindVertexArray(m_vertexInput.GetHandle());
    SCOPE_GUARD(glBindVertexArray(0));

    glUseProgram(m_shader.GetHandle());
    SCOPE_GUARD(glUseProgram(0));

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    // Batch shapes and draw them.
    int verticesBatched = 0;

    for(int i = 0; i < count; ++i)
    {
        const Rectangle& rectangle = data[i];

        // Create shape vertices.
        glm::vec4 rect(rectangle.bottomleft, rectangle.topright);

        Vertex shapeVertices[ShapeVerticeCount] =
        {
            { glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.z, rect.y), glm::vec2(1.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), rectangle.color },

            { glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), rectangle.color },
            { glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
            { glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 1.0f), rectangle.color },
        };

        // Copy vertices to a temporary buffer.
        memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

        verticesBatched += ShapeVerticeCount;

        // Determine if we have to draw the batch.
        bool drawBatch = false;

        if(m_bufferSize - verticesBatched < ShapeVerticeCount)
        {
            // We reached the buffer size.
            drawBatch = true;
        }
        else
        if(i == count - 1)
        {
            // We reached the last element.
            drawBatch = true;
        }

        // Draw the batch if needed.
        if(drawBatch)
        {
            // Update vertex buffer.
            m_vertexBuffer.Update(m_bufferData);

            // Draw shapes.
            glDrawArrays(GL_TRIANGLES, 0, verticesBatched);

            // Reset batch counter.
            verticesBatched = 0;
        }
    }
}

void BasicRenderer::DrawCircles(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    if(data == nullptr)
        return;

    if(count <= 0)
        return;

    // Draw based on mode.
    if(style.drawMode == DrawMode::Line)
    {
        DrawCirclesLine(style, data, count, transform);
    }
    else
    if(style.drawMode == DrawMode::Fill)
    {
        DrawCirclesFill(style, data, count, transform);
    }
}

void BasicRenderer::DrawCirclesLine(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Not implemented yet.
}

void BasicRenderer::DrawCirclesFill(const CircleStyle& style, const Circle* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    // Not implemented yet.
}

void BasicRenderer::DrawSprites(const SpriteStyle& style, const Sprite* data, int count, const glm::mat4& transform)
{
    if(!m_initialized)
        return;

    if(data == nullptr)
        return;

    if(count <= 0)
        return;

    // Minimum buffer size needed.
    const int ShapeVerticeCount = 6;
    assert(m_bufferSize >= ShapeVerticeCount);

    // Bind rendering states.
    if(style.alphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    SCOPE_GUARD(glDisable(GL_BLEND));

    glActiveTexture(GL_TEXTURE0);
    SCOPE_GUARD(glBindTexture(GL_TEXTURE_2D, 0));

    glBindVertexArray(m_vertexInput.GetHandle());
    SCOPE_GUARD(glBindVertexArray(0));

    glUseProgram(m_shader.GetHandle());
    SCOPE_GUARD(glUseProgram(0));

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);

    // Batch shapes and draw them.
    const Texture* currentTexture = nullptr;
    int verticesBatched = 0;

    for(int i = 0; i < count; ++i)
    {
        const Sprite& sprite = data[i];

        // Create shape vertices.
        glm::vec4 rect(sprite.position, sprite.position + sprite.size);

        Vertex shapeVertices[ShapeVerticeCount] =
        {
            { glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), sprite.color },
            { glm::vec2(rect.z, rect.y), glm::vec2(1.0f, 0.0f), sprite.color },
            { glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), sprite.color },

            { glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), sprite.color },
            { glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), sprite.color },
            { glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 1.0f), sprite.color },
        };

        // Copy vertices to a temporary buffer.
        memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

        verticesBatched += ShapeVerticeCount;

        // Determine if we have to draw the batch.
        bool drawBatch = false;

        if(m_bufferSize - verticesBatched < ShapeVerticeCount)
        {
            // We reached the buffer size.
            drawBatch = true;
        }
        else
        if(i == count - 1)
        {
            // We reached the last element.
            drawBatch = true;
        }
        else
        if(sprite.texture != data[i + 1].texture)
        {
            // Texture is going to change, we have to draw.
            drawBatch = true;
        }

        // Draw the batch if needed.
        if(drawBatch)
        {
            // Update vertex buffer.
            m_vertexBuffer.Update(m_bufferData);

            // Bind the texture.
            if(sprite.texture != nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, sprite.texture->GetHandle());
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, Main::GetBlankTexture().GetHandle());
            }

            // Draw shapes.
            glDrawArrays(GL_TRIANGLES, 0, verticesBatched);

            // Reset batch counter.
            verticesBatched = 0;
        }
    }
}
