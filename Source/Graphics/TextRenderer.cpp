#include "Precompiled.hpp"
#include "TextRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "Font.hpp"
#include "MainContext.hpp"

namespace Console
{
    ConsoleVariable debugTextRender("debug_textrender", false, "Enables text render debugging.");
}

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a text renderer! "

    // Cursor blink time.
    const float CursorBlinkTime = 2.0f * 0.530f;
}

//
// Draw State
//

class DrawState
{
public:
    DrawState() :
        m_drawInfo(nullptr),
        m_textLength(0),
        m_textLine(0),
        m_textIterator(nullptr),
        m_textEnd(nullptr),
        m_characterIndex(-1),
        m_characterCurrent('\0'),
        m_characterPrevious('\0'),
        m_wordProcessed(true),
        m_wordWrap(false),
        m_drawPosition(0.0f, 0.0f),
        m_drawArea(0.0f, 0.0f, 0.0f, 0.0f),
        m_glyph(nullptr),
        m_cursorPosition(0.0f, 0.0f),
        m_cursorPresent(false),
        m_boundingBox(0.0f, 0.0f, 0.0f, 0.0f),
        m_initialized(false)
    {
    }

    bool Initialize(const TextRenderer::DrawInfo& info, const char* text)
    {
        // Validate arguments.
        if(info.font == nullptr)
            return false;

        if(info.size.x < 0.0f)
            return false;

        if(info.size.y < 0.0f)
            return false;

        if(text == nullptr)
            return false;

        m_drawInfo = &info;

        // Calculate text size.
        size_t textSize = strlen(text);

        // Check if the text is valid.
        if(!utf8::is_valid(text, text + textSize))
            return false;

        // Calculate text length.
        m_textLength = utf8::distance(text, text + textSize);

        // Set text iterators.
        m_textIterator = text;
        m_textEnd = text + textSize;

        // Check if we want to wrap the text.
        if(info.size.x > 0.0f)
            m_wordWrap = true;

        // Set initial drawing position.
        m_drawPosition.x = info.position.x;
        m_drawPosition.y = info.position.y - info.font->GetAscender();

        // Set initial cursor position.
        if(m_drawInfo->cursorIndex >= 0 && m_drawInfo->cursorIndex <= m_textLength)
        {
            m_cursorPosition = m_drawPosition;
            m_cursorPresent = true;
        }

        // Sucsess!
        m_initialized = true;

        return true;
    }

    bool ProcessNext()
    {
        assert(m_initialized);

        if(!m_initialized)
            return false;

        // Check if we are done processing the text.
        if(this->IsDone())
            return false;

        // Advance position of previously drawn glyph.
        if(m_characterIndex >= 0 && m_glyph != nullptr)
        {
            AdvancePosition(m_glyph);
        }

        // Reset current glyph pointer.
        m_glyph = nullptr;

        // Save the previously processed character.
        m_characterPrevious = m_characterCurrent;

        // Get the next UTF-8 encoded character.
        const char* currentIterator = m_textIterator;
        m_characterCurrent = utf8::next(m_textIterator, m_textEnd);

        // Increment the character index.
        ++m_characterIndex;

        // Handle special characters.
        if(m_characterCurrent == '\n')
        {
            // Move to the next line.
            if(m_characterIndex != m_textLength - 1)
            {
                MoveNextLine();
            }

            // Skip this character entirely.
            return true;
        }
        else
        if(m_characterCurrent == ' ')
        {
            // Process a new word.
            m_wordProcessed = false;
        }
        
        // Process next word.
        if(m_characterCurrent != ' ' && m_wordProcessed == false)
        {
            // Check if a word will fit in the current line.
            if(m_wordWrap)
            {
                float wordSize = 0.0f;

                FT_ULong wordCharacterPrevious = ' ';
                const char* wordCharacterIterator = currentIterator;

                while(wordCharacterIterator != m_textEnd)
                {
                    // Get the next word character.
                    FT_ULong wordCharacterCurrent = utf8::next(wordCharacterIterator, m_textEnd);

                    // Check if we reached an end of a word.
                    if(wordCharacterCurrent == ' ')
                        break;

                    // We can't check if the first word will fit, it will cause problems.
                    assert(m_characterIndex != 0);

                    // Get glyph description.
                    const Glyph* glyph = m_drawInfo->font->GetGlyph(wordCharacterCurrent);
                    assert(glyph != nullptr);

                    // Get glyph kerning.
                    int kerning = m_drawInfo->font->GetKerning(wordCharacterPrevious, wordCharacterCurrent);

                    // Check if the word will fit.
                    wordSize += glyph->advance.x + kerning;

                    if(m_drawPosition.x - m_drawInfo->position.x + wordSize > m_drawInfo->size.x)
                    {
                        // Don't draw last space for debug base line.
                        if(m_drawInfo->debug)
                        {
                            const Glyph* glyphSpace = m_drawInfo->font->GetGlyph(' ');
                            assert(glyphSpace != nullptr);

                            m_drawPosition.x -= glyphSpace->advance.x;
                        }

                        // Move to the next line.
                        MoveNextLine();

                        break;
                    }

                    // Save current character as previous for the next loop iteration.
                    wordCharacterPrevious = wordCharacterCurrent;
                }
            }

            m_wordProcessed = true;
        }

        // Get glyph description.
        const Glyph* glyph = m_drawInfo->font->GetGlyph(m_characterCurrent);
        assert(glyph != nullptr);

        m_glyph = glyph;

        // Apply glyph kerning.
        if(m_characterIndex != 0)
        {
            int kerning = m_drawInfo->font->GetKerning(m_characterPrevious, m_characterCurrent);
            m_drawPosition.x += kerning;
        }

        // Check if it's the cursor position.
        if(m_drawInfo->cursorIndex == m_characterIndex + 1)
        {
            m_cursorPosition.x = m_drawPosition.x + glyph->advance.x;
            m_cursorPosition.y = m_drawPosition.y + glyph->advance.y;
        }

        // Set initial bounding box.
        if(m_characterIndex == 0)
        {
            m_boundingBox.x = m_boundingBox.y = std::numeric_limits<float>::max();
            m_boundingBox.z = m_boundingBox.w = std::numeric_limits<float>::min();
        }

        // Calculate current bounding box.
        m_boundingBox.x = std::min(m_boundingBox.x, m_drawPosition.x + glyph->offset.x);
        m_boundingBox.y = std::min(m_boundingBox.y, m_drawPosition.y + glyph->offset.y);
        m_boundingBox.z = std::max(m_boundingBox.z, m_drawPosition.x + glyph->offset.x + glyph->size.x);
        m_boundingBox.w = std::max(m_boundingBox.w, m_drawPosition.y + glyph->offset.y + glyph->size.y);

        // Calculate current draw area.
        m_drawArea.x = m_drawInfo->position.x;
        m_drawArea.y = m_drawPosition.y + m_drawInfo->font->GetDescender();

        if(m_wordWrap)
        {
            m_drawArea.z = m_drawInfo->position.x + m_drawInfo->size.x;
        }
        else
        {
            m_drawArea.z = std::max(m_drawArea.z, m_drawPosition.x + glyph->advance.x);
            //m_drawArea.z = m_boundingBox.z;
        }

        m_drawArea.w = m_drawInfo->position.y;

        return false;
    }

    bool IsDone() const
    {
        return m_textIterator == m_textEnd;
    }

private:
    void MoveNextLine()
    {
        if(!m_initialized)
            return;

        // Move to the next line.
        m_drawPosition.x = m_drawInfo->position.x;
        m_drawPosition.y -= m_drawInfo->font->GetLineSpacing();

        // Increase line counter.
        ++m_textLine;
    }

    void AdvancePosition(const Glyph* glyph)
    {
        assert(glyph != nullptr);

        if(!m_initialized)
            return;

        if(glyph == nullptr)
            return;

        // Advance position for next glyph.
        m_drawPosition.x += glyph->advance.x;
        m_drawPosition.y += glyph->advance.y;
    }

public:
    int GetCurrentLine() const
    {
        return m_textLine;
    }

    FT_ULong GetCurrentCharacter() const
    {
        return m_characterCurrent;
    }

    FT_ULong GetPreviousCharacter() const
    {
        return m_characterPrevious;
    }

    const glm::vec2& GetDrawPosition() const
    {
        return m_drawPosition;
    }

    const glm::vec4& GetDrawArea() const
    {
        return m_drawArea;
    }

    const Glyph* GetCurrentGlyph() const
    {
        return m_glyph;
    }

    const glm::vec2& GetCursorPosition() const
    {
        return m_cursorPosition;
    }

    const glm::vec4& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    bool IsWordWrapEnabled() const
    {
        return m_wordWrap;
    }

    bool IsCursorPresent() const
    {
        return m_cursorPresent;
    }

private:
    // Text draw info.
    const TextRenderer::DrawInfo* m_drawInfo;

    // Text iterators.
    int         m_textLength;
    int         m_textLine;
    const char* m_textIterator;
    const char* m_textEnd;

    // Text character.
    int      m_characterIndex;
    FT_ULong m_characterCurrent;
    FT_ULong m_characterPrevious;

    // Text wrapping.
    bool m_wordProcessed;
    bool m_wordWrap;

    // Draw position.
    glm::vec2 m_drawPosition;

    // Text draw area.
    glm::vec4 m_drawArea;

    // Current character glyph.
    const Glyph* m_glyph;

    // Text cursor.
    glm::vec2 m_cursorPosition;
    bool      m_cursorPresent;

    // Text bounding box.
    glm::vec4 m_boundingBox;

    bool m_initialized;
};

//
// Text Renderer
//

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
    if(!m_shader.Load(Main::WorkingDir() + "Data/Shaders/Glyph.glsl"))
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

TextRenderer::DrawMetrics TextRenderer::Measure(const DrawInfo& info, const char* text)
{
    DrawMetrics output;

    if(!m_initialized)
        return output;

    // Initialize the text draw state.
    DrawState state;
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

void TextRenderer::Draw(const DrawInfo& info, const glm::mat4& transform, const char* text)
{
    if(!m_initialized)
        return;
    
    // Initialize the text draw state.
    DrawState state;
    if(!state.Initialize(info, text))
        return;

    // Debug drawing.
    float baselineMaxWidth = state.GetDrawPosition().x;

    glm::vec2 baselineBegin(state.GetDrawPosition());
    glm::vec2 baselineEnd(state.GetDrawPosition());

    std::vector<ShapeRenderer::Line> debugLines;
    std::vector<ShapeRenderer::Rectangle> debugRectangles;

    auto AddDebugBaseline = [&]() -> void
    {
        assert(info.debug || Console::debugTextRender);

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
        assert(info.debug || Console::debugTextRender);

        ShapeRenderer::Rectangle rectangle;
        rectangle.position = position;
        rectangle.size = size;
        rectangle.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

        debugRectangles.push_back(rectangle);
    };

    // Update font texture atlas.
    info.font->CacheGlyphs(text);
    info.font->UpdateAtlasTexture();

    // Calculate pixel size of the atlas texture.
    glm::vec2 pixelSize(1.0f / info.font->GetAtlasWidth(), 1.0f / info.font->GetAtlasHeight());

    // Bind render states.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, info.font->GetTexture()->GetHandle());

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glUseProgram(m_shader.GetHandle());
    glBindVertexArray(m_vertexInput.GetHandle());

    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);
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
            if(info.debug)
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
            glyphData.position.x = (float)(state.GetDrawPosition().x + glyph->offset.x);
            glyphData.position.y = (float)(state.GetDrawPosition().y + glyph->offset.y);
            glyphData.size.x = (float)glyph->size.x;
            glyphData.size.y = (float)glyph->size.y;
            glyphData.scale.x = 1.0f;
            glyphData.scale.y = 1.0f;
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
            if(info.debug)
            {
                AddDebugGlyph(glyphData.position, glyphData.size);
            }

            // Update baseline end.
            baselineEnd.x = state.GetDrawPosition().x + glyph->advance.x;
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
        cursorLine.begin.y = state.GetCursorPosition().y + info.font->GetAscender();
        cursorLine.end.x = state.GetCursorPosition().x;
        cursorLine.end.y = state.GetCursorPosition().y + info.font->GetDescender();

        Main::ShapeRenderer().DrawLines(&cursorLine, 1, transform);
    }

    // Flush debug draw.
    if(info.debug || Console::debugTextRender)
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
            rectangle.size.x = state.GetDrawArea().z - rectangle.position.x;
            rectangle.size.y = state.GetDrawArea().w - rectangle.position.y;

            Main::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
        }
    }
}
