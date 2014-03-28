#include "Precompiled.hpp"
#include "TextRenderer.hpp"
#include "TextDrawState.hpp"
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
        { &m_instanceBuffer, VertexAttributeTypes::Float4 }, // Color
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
    TextDrawState state;
    if(!state.Initialize(info, text))
        return output;

    // Process all characters.
    while(!state.IsDone())
    {
        state.ProcessNext();
    }

    // Return draw metrics.
    output.boundingBox = state.GetBoundingBox();
    output.drawingArea = state.GetDrawArea();
    output.lines = state.GetCurrentLine() + 1;

    return output;
}

void TextRenderer::Draw(const TextDrawInfo& info, const glm::mat4& transform, const char* text)
{
    if(!m_initialized)
        return;
    
    // Initialize the text draw state.
    TextDrawState state;
    if(!state.Initialize(info, text))
        return;

    // Debug drawing.
    bool debugDraw = info.debug || Console::debugTextRenderer;

    float baselineMaxWidth = state.GetDrawPosition().x;

    glm::vec2 baselineBegin(state.GetDrawPosition());
    glm::vec2 baselineEnd(state.GetDrawPosition());

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
    info.font->CacheGlyphs(state.GetProcessedString().c_str());
    info.font->UpdateAtlasTexture();

    // Calculate pixel size of the atlas texture.
    glm::vec2 pixelSize(1.0f / info.font->GetAtlasWidth(), 1.0f / info.font->GetAtlasHeight());

    // Bind render states.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    while(!state.IsDone())
    {
        // Process next character.
        if(state.ProcessNext())
            continue;

        // Check if we moved to a next line.
        if(currentLine != state.GetCurrentLine())
        {
            // Add debug baseline.
            if(debugDraw)
            {
                AddDebugBaseline();
            }

            // Reset baseline position.
            baselineBegin = state.GetDrawPosition();
            baselineEnd = state.GetDrawPosition();

            // Set current line.
            currentLine = state.GetCurrentLine();
        }

        // Get current glyph.
        const Glyph* glyph = state.GetCurrentGlyph();

        // Draw character glyphs.
        if(state.GetCurrentCharacter() != ' ')
        {
            // Fill a glyph instance data.
            GlyphData& glyphData = m_bufferData[glyphsBatched];
            glyphData.position.x = state.GetDrawPosition().x + glyph->offset.x * state.GetFontScale();
            glyphData.position.y = state.GetDrawPosition().y + glyph->offset.y * state.GetFontScale();
            glyphData.size.x = (float)glyph->size.x;
            glyphData.size.y = (float)glyph->size.y;
            glyphData.scale.x = state.GetFontScale();
            glyphData.scale.y = state.GetFontScale();
            glyphData.texture.x = (float)glyph->position.x;
            glyphData.texture.y = (float)glyph->position.y;
            glyphData.color = info.color;

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
            baselineEnd.x = state.GetDrawPosition().x + glyph->advance.x * state.GetFontScale();
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
    if(state.IsCursorPresent() && m_cursorBlinkTime < CursorBlinkTime * 0.5f)
    {
        ShapeRenderer::Line cursorLine;
        cursorLine.color = info.color;
        cursorLine.begin.x = state.GetCursorPosition().x;
        cursorLine.begin.y = state.GetCursorPosition().y + info.font->GetAscender() * state.GetFontScale();
        cursorLine.end.x = state.GetCursorPosition().x;
        cursorLine.end.y = state.GetCursorPosition().y + info.font->GetDescender() * state.GetFontScale();

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
            rectangle.position.x = state.GetBoundingBox().x;
            rectangle.position.y = state.GetBoundingBox().y;
            rectangle.size.x = state.GetBoundingBox().z - rectangle.position.x;
            rectangle.size.y = state.GetBoundingBox().w - rectangle.position.y;

            Main::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
        }

        // Draw text area.
        {
            ShapeRenderer::Rectangle rectangle;
            rectangle.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            rectangle.position.x = state.GetDrawArea().x;
            rectangle.position.y = state.GetDrawArea().y;
            rectangle.size.x = state.GetDrawArea().z - rectangle.position.x + 1.0f;
            rectangle.size.y = state.GetDrawArea().w - rectangle.position.y + 1.0f;

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
