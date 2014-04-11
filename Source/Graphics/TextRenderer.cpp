#include "Precompiled.hpp"
#include "TextRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "MainContext.hpp"
#include "Font.hpp"

namespace Console
{
    ConsoleVariable debugTextRenderer("debug_textrenderer", false, "Enables text render debugging.");
}

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a text renderer! "

    // Cursor blink time.
    const float CursorBlinkTime = 2.0f * 0.530f;
}

TextRenderer::TextRenderer() :
    m_bufferData(nullptr),
    m_bufferSize(0),
    m_shader(),
    m_vertexBuffer(),
    m_instanceBuffer(),
    m_vertexInput(),
    m_cursorBlinkTime(0.0f),
    m_initialized(false)
{
}

TextRenderer::~TextRenderer()
{
    Cleanup();
}

bool TextRenderer::Initialize(int bufferSize)
{
    Cleanup();

    // Validate arguments.
    if(bufferSize <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"bufferSize\" is invalid.";
        return false;
    }

    m_bufferSize = bufferSize;

    // Allocate buffer data for glyph instances.
    m_bufferData = new GlyphData[m_bufferSize];

    // Load glyph shader.
    if(!m_shader.Load("Data/Shaders/Glyph.glsl"))
    {
        Log() << LogInitializeError() << "Couldn't load a shader.";
        Cleanup();
        return false;
    }

    // Create the vertex buffer.
    struct Vertex
    {
        glm::vec2 position;
    };

    Vertex vertices[4] =
    {
        { glm::vec2(0.0f, 0.0f) },
        { glm::vec2(1.0f, 0.0f) },
        { glm::vec2(1.0f, 1.0f) },
        { glm::vec2(0.0f, 1.0f) },
    };

    if(!m_vertexBuffer.Initialize(sizeof(Vertex), StaticArraySize(vertices), &vertices, GL_STATIC_DRAW))
    {
        Log() << LogInitializeError() << "Couldn't create a vertex buffer.";
        Cleanup();
        return false;
    }

    // Create an instance buffer.
    if(!m_instanceBuffer.Initialize(sizeof(GlyphData), m_bufferSize, nullptr, GL_DYNAMIC_DRAW))
    {
        Log() << LogInitializeError() << "Couldn't create an instance buffer.";
        Cleanup();
        return false;
    }

    // Vertex input.
    VertexAttribute vertexAttributes[] =
    {
        // Vertex
        { &m_vertexBuffer, VertexAttributeTypes::Float2 },   // Position

        // Instance
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Position
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Size
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Scale
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Texture

        { &m_instanceBuffer, VertexAttributeTypes::Float4 }, // Body Color
        { &m_instanceBuffer, VertexAttributeTypes::Float4 }, // Outline Color
        { &m_instanceBuffer, VertexAttributeTypes::Float4 }, // Glow Color
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Outline Range
        { &m_instanceBuffer, VertexAttributeTypes::Float2 }, // Glow Range
    };

    if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
    {
        Log() << LogInitializeError() << "Couldn't create a vertex input.";
        Cleanup();
        return false;
    }

    return m_initialized = true;
}

void TextRenderer::Cleanup()
{
    m_drawState.Cleanup();

    delete[] m_bufferData;
    m_bufferData = nullptr;

    m_bufferSize = 0;

    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_instanceBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_cursorBlinkTime = 0.0f;

    m_initialized = false;
}

void TextRenderer::UpdateCursorBlink(float dt)
{
    if(!m_initialized)
        return;

    m_cursorBlinkTime += dt;
    m_cursorBlinkTime = std::fmod(m_cursorBlinkTime, CursorBlinkTime);
}

void TextRenderer::ResetCursorBlink()
{
    if(!m_initialized)
        return;

    m_cursorBlinkTime = 0.0f;
}

TextDrawMetrics TextRenderer::Measure(const TextDrawInfo& info, const char* text)
{
    TextDrawMetrics output;

    if(!m_initialized)
        return output;

    // Initialize the text draw state.
    if(!m_drawState.Initialize(info, text))
        return output;

    // Return draw metrics.
    output.textArea = m_drawState.GetTextArea();
    output.boundingBox = m_drawState.GetBoundingBox();
    output.lines = m_drawState.GetCurrentLine() + 1;

    return output;
}

void TextRenderer::Draw(const TextDrawInfo& info, const glm::mat4& transform, const char* text)
{
    if(!m_initialized)
        return;
    
    // Initialize the text draw state.
    if(!m_drawState.Initialize(info, text))
        return;

    // Debug drawing.
    bool debugDraw = info.debug || Console::debugTextRenderer;

    float baselineMaxWidth = m_drawState.GetDrawPosition().x + m_drawState.GetAlignOffset().x;

    glm::vec2 baselineBegin(m_drawState.GetDrawPosition() + m_drawState.GetAlignOffset());
    glm::vec2 baselineEnd(m_drawState.GetDrawPosition() + m_drawState.GetAlignOffset());

    std::vector<ShapeRenderer::Line> debugLines;
    std::vector<ShapeRenderer::Rectangle> debugRectangles;

    auto AddDebugBaseline = [&]() -> void
    {
        assert(debugDraw);

        // Set max reached baseline width.
        baselineMaxWidth = std::max(baselineMaxWidth, baselineEnd.x);

        // Add line shape.
        ShapeRenderer::Line line;
        line.begin = baselineBegin;
        line.end = baselineEnd;
        line.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        debugLines.push_back(line);
    };

    auto AddDebugGlyph = [&](const glm::vec2& position, const glm::vec2& size)
    {
        assert(debugDraw);

        ShapeRenderer::Rectangle rectangle;
        rectangle.position = position;
        rectangle.size = size;
        rectangle.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

        debugRectangles.push_back(rectangle);
    };

    // Update font texture atlas.
    info.font->CacheGlyphs(m_drawState.GetProcessedString().c_str());
    info.font->UpdateAtlasTexture();

    // Calculate pixel size of the atlas texture.
    glm::vec2 pixelSize(1.0f / info.font->GetAtlasWidth(), 1.0f / info.font->GetAtlasHeight());

    // Use premultiplied alpha.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind render states.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, info.font->GetAtlasTexture()->GetHandle());

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(m_shader.GetHandle());
    glBindVertexArray(m_vertexInput.GetHandle());

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("fontTexture"), 0);
    glUniform2fv(m_shader.GetUniform("texturePixelSize"), 1, glm::value_ptr(pixelSize));

    auto ResetRenderState = []()
    {
        glBindVertexArray(0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    };

    // Method that draws buffered characters.
    int glyphsBatched = 0;

    auto DrawBatchedGlyphs = [&]()
    {
        // Update the instance buffer.
        m_instanceBuffer.Update(&m_bufferData[0], glyphsBatched);

        // Draw glyph quads.
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_vertexBuffer.GetElementCount(), glyphsBatched);

        // Reset the counter.
        glyphsBatched = 0;
    };

    // Process and draw text characters.
    int currentLine = 0;

    while(!m_drawState.IsDone())
    {
        // Process next character.
        if(!m_drawState.ProcessNext())
            continue;

        // Check if we moved to a next line.
        if(currentLine != m_drawState.GetCurrentLine())
        {
            // Add debug baseline.
            if(debugDraw)
            {
                AddDebugBaseline();
            }

            // Reset baseline position.
            baselineBegin = m_drawState.GetDrawPosition() + m_drawState.GetAlignOffset();
            baselineEnd = m_drawState.GetDrawPosition() + m_drawState.GetAlignOffset();

            // Set current line.
            currentLine = m_drawState.GetCurrentLine();
        }

        // Get current glyph.
        const Glyph* glyph = m_drawState.GetCurrentGlyph();

        // Draw character glyphs.
        if(m_drawState.GetCurrentCharacter() != ' ')
        {
            // Fill a glyph instance data.
            GlyphData& glyphData = m_bufferData[glyphsBatched];
            glyphData.position.x = m_drawState.GetDrawPosition().x + m_drawState.GetAlignOffset().x + glyph->offset.x * m_drawState.GetFontScale();
            glyphData.position.y = m_drawState.GetDrawPosition().y + m_drawState.GetAlignOffset().y + glyph->offset.y * m_drawState.GetFontScale();
            glyphData.size.x = (float)glyph->size.x;
            glyphData.size.y = (float)glyph->size.y;
            glyphData.scale.x = m_drawState.GetFontScale();
            glyphData.scale.y = m_drawState.GetFontScale();
            glyphData.texture.x = (float)glyph->position.x;
            glyphData.texture.y = (float)glyph->position.y;
            glyphData.bodyColor = info.bodyColor;
            glyphData.outlineColor = info.outlineColor;
            glyphData.glowColor = info.glowColor;
            glyphData.outlineRange = info.outlineRange;
            glyphData.glowRange = info.glowRange;

            ++glyphsBatched;

            // Draw if we reached the buffer size.
            if(glyphsBatched == m_bufferSize)
            {
                DrawBatchedGlyphs();
            }

            // Add debug glyph rectangle.
            if(debugDraw)
            {
                AddDebugGlyph(glyphData.position, glyphData.size * glyphData.scale);
            }

            // Update baseline end.
            baselineEnd.x = m_drawState.GetDrawPosition().x + m_drawState.GetAlignOffset().x + glyph->advance.x * m_drawState.GetFontScale();
        }
    }

    // Draw any remaining glyphs that were batched.
    if(glyphsBatched != 0)
    {
        DrawBatchedGlyphs();
    }

    // Unbind render states.
    ResetRenderState();

    // Draw text cursor.
    if(m_drawState.IsCursorPresent() && m_cursorBlinkTime < CursorBlinkTime * 0.5f)
    {
        ShapeRenderer::Line cursorLine;
        cursorLine.color = info.bodyColor;
        cursorLine.begin.x = m_drawState.GetCursorPosition().x + m_drawState.GetAlignOffset().x;
        cursorLine.begin.y = m_drawState.GetCursorPosition().y + m_drawState.GetAlignOffset().y + info.font->GetAscender() * m_drawState.GetFontScale();
        cursorLine.end.x = m_drawState.GetCursorPosition().x + m_drawState.GetAlignOffset().x;
        cursorLine.end.y = m_drawState.GetCursorPosition().y + m_drawState.GetAlignOffset().y + info.font->GetDescender() * m_drawState.GetFontScale();

        Main::ShapeRenderer().DrawLines(&cursorLine, 1, transform);
    }

    // Flush debug draw.
    if(debugDraw)
    {
        // Draw glyph rectangles.
        if(!debugRectangles.empty())
        {
            Main::ShapeRenderer().DrawRectangles(&debugRectangles[0], debugRectangles.size(), transform);
        }

        // Add last base line.
        AddDebugBaseline();

        // Draw all base lines.
        if(!debugLines.empty())
        {
            Main::ShapeRenderer().DrawLines(&debugLines[0], debugLines.size(), transform);
        }

        // Draw bounding box.
        {
            ShapeRenderer::Rectangle rectangle;
            rectangle.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            rectangle.position.x = m_drawState.GetBoundingBox().x + m_drawState.GetAlignOffset().x;
            rectangle.position.y = m_drawState.GetBoundingBox().y + m_drawState.GetAlignOffset().y;
            rectangle.size.x = m_drawState.GetBoundingBox().z - m_drawState.GetBoundingBox().x;
            rectangle.size.y = m_drawState.GetBoundingBox().w - m_drawState.GetBoundingBox().y;

            Main::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
        }

        // Draw text area.
        {
            ShapeRenderer::Rectangle rectangle;
            rectangle.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            rectangle.position.x = m_drawState.GetTextArea().x + m_drawState.GetAlignOffset().x;
            rectangle.position.y = m_drawState.GetTextArea().y + m_drawState.GetAlignOffset().y;
            rectangle.size.x = m_drawState.GetTextArea().z - m_drawState.GetTextArea().x + 1.0f;
            rectangle.size.y = m_drawState.GetTextArea().w - m_drawState.GetTextArea().y + 1.0f;

            Main::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
        }

        // Draw origin.
        {
            ShapeRenderer::Line lines[2];
            lines[0].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            lines[0].begin = info.position + glm::vec2(-100.0f, 0.0f);
            lines[0].end   = info.position + glm::vec2( 100.0f, 0.0f);

            lines[1].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            lines[1].begin = info.position + glm::vec2(0.0f, -100.0f);
            lines[1].end   = info.position + glm::vec2(0.0f,  100.0f);

            Main::ShapeRenderer().DrawLines(&lines[0], 2, transform);
        }
    }
}
