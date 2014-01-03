#include "Precompiled.hpp"
#include "TextRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "Font.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a text renderer! "

    // Index type.
    typedef unsigned int Index;

    // Cursor blink time.
    const float CursorBlinkTime = 2.0f * 0.530f;
}

TextRenderer::TextRenderer() :
    m_vertexData(nullptr),
    m_bufferSize(0),
    m_shader(),
    m_vertexBuffer(),
    m_indexBuffer(),
    m_vertexInput(),
    m_cursorTime(0.0f),
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

    // Allocate vertex data buffer for glyph quads.
    m_vertexData = new Vertex[m_bufferSize * 4];

    // Load text shader.
    if(!m_shader.Load(Context::WorkingDir() + "Data/Shaders/Text.glsl"))
    {
        Log() << LogInitializeError() << "Couldn't load a shader.";
        Cleanup();
        return false;
    }

    // Create the vertex buffer.
    if(!m_vertexBuffer.Initialize(sizeof(Vertex), m_bufferSize * 4, nullptr, GL_DYNAMIC_DRAW))
    {
        Log() << LogInitializeError() << "Couldn't create a vertex buffer.";
        Cleanup();
        return false;
    }

    // Create index data for quads.
    std::vector<Index> indexData;
    indexData.resize(m_bufferSize * 6);

    for(int i = 0; i < m_bufferSize; ++i)
    {
        indexData[i * 6 + 0] = i * 4 + 0;
        indexData[i * 6 + 1] = i * 4 + 1;
        indexData[i * 6 + 2] = i * 4 + 2;

        indexData[i * 6 + 3] = i * 4 + 2;
        indexData[i * 6 + 4] = i * 4 + 3;
        indexData[i * 6 + 5] = i * 4 + 0;
    }

    // Create the index buffer.
    if(!m_indexBuffer.Initialize(sizeof(Index), indexData.size(), &indexData[0]))
    {
        Log() << LogInitializeError() << "Couldn't create an index buffer.";
        Cleanup();
        return false;
    }

    // Vertex input.
    VertexAttribute vertexAttributes[] =
    {
        { 0, &m_vertexBuffer, VertexAttributeTypes::Float2 },
        { 1, &m_vertexBuffer, VertexAttributeTypes::Float2 },
        { 2, &m_vertexBuffer, VertexAttributeTypes::Float4 },
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
    delete[] m_vertexData;
    m_vertexData = nullptr;

    m_bufferSize = 0;

    m_shader.Cleanup();
    m_vertexBuffer.Cleanup();
    m_indexBuffer.Cleanup();
    m_vertexInput.Cleanup();

    m_cursorTime = 0.0f;

    m_initialized = false;
}

void TextRenderer::Update(float dt)
{
    m_cursorTime += dt;
    m_cursorTime = fmod(m_cursorTime, CursorBlinkTime);
}

void TextRenderer::Draw(const DrawInfo& info, const glm::mat4& transform, const char* text)
{
    if(!m_initialized)
        return;

    // Validate arguments.
    if(info.font == nullptr)
        return;

    if(info.size.x < 0.0f)
        return;

    if(info.size.y < 0.0f)
        return;

    if(text == nullptr)
        return;

    // Check text string.
    size_t textSize = strlen(text);

    if(!utf8::is_valid(text, text + textSize))
        return;

    // Current drawing position.
    glm::vec2 baselinePosition;
    baselinePosition.x = info.position.x;
    baselinePosition.y = info.position.y - info.font->GetAscender();

    // Text cursor position (to be determined).
    glm::vec2 cursorPosition = baselinePosition;

    // Debug drawing.
    glm::vec2 baselineBegin(baselinePosition);

    std::vector<ShapeRenderer::Line> debugLines;
    std::vector<ShapeRenderer::Rectangle> debugRectangles;

    auto AddDebugBaseLine = [&](const glm::vec2& baselineEnd) -> void
    {
        assert(info.debug);

        ShapeRenderer::Line line;
        line.begin = baselineBegin;
        line.end = baselineEnd;
        line.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        debugLines.push_back(line);
    };

    auto AddDebugGlyphRectangle = [&](const glm::vec2& position, const glm::vec2& size)
    {
        assert(info.debug);

        ShapeRenderer::Rectangle rectangle;
        rectangle.position = position;
        rectangle.size = size;
        rectangle.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

        debugRectangles.push_back(rectangle);
    };

    // Method that moves drawing position to the next line.
    auto MoveNextLine = [&]()
    {
        // Draw debug base line.
        if(info.debug)
        {
            AddDebugBaseLine(baselinePosition);
        }

        // Move to the next line.
        baselinePosition.x = info.position.x;
        baselinePosition.y -= info.font->GetLineSpacing();

        // Set new baseline begining.
        if(info.debug)
        {
            baselineBegin = baselinePosition;
        }
    };

    // Method that advances drawing position after drawing a character.
    auto AdvanceBaseline = [&](int index, const Glyph* glyph)
    {
        // Advance position for next glyph.
        baselinePosition.x += glyph->advance.x;
        baselinePosition.y += glyph->advance.y;

        // Save the position of the cursor for it to be drawn later.
        if(info.cursorIndex == index + 1)
        {
            cursorPosition = baselinePosition;
        }
    };

    // Update font texture atlas.
    info.font->CacheGlyphs(text);
    info.font->UpdateAtlasTexture();

    // Bind render states.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, info.font->GetTexture()->GetHandle());

    glUseProgram(m_shader.GetHandle());
    glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
    glUniform1i(m_shader.GetUniform("texture"), 0);
    
    glBindVertexArray(m_vertexInput.GetHandle());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.GetHandle());

    // Calculate pixel size of the atlas texture.
    glm::vec2 pixelSize(1.0f / info.font->GetAtlasWidth(), 1.0f / info.font->GetAtlasHeight());

    // Method that draws buffered characters.
    int charactersBuffered = 0;

    auto DrawBufferedCharacters = [&]()
    {
        // Update the vertex buffer.
        m_vertexBuffer.Update(&m_vertexData[0]);

        // Draw character quads.
        glDrawElements(GL_TRIANGLES, charactersBuffered * 6, m_indexBuffer.GetElementType(), (void*)0);

        // Reset the counter.
        charactersBuffered = 0;
    };

    // Draw characters.
    size_t textLength = utf8::distance(text, text + textSize);

    const char* it = text;
    const char* end = text + textSize;

    bool wordProcessed = true;

    for(size_t i = 0; i < textLength; ++i)
    {
        // Get next UTF-8 encoded character.
        FT_ULong character = utf8::next(it, end);

        // Check if it's one of the special characters.
        if(character == '\n')
        {
            // Move to the next line.
            if(i != textLength - 1)
            {
                MoveNextLine();
            }

            continue;
        }
        else
        if(character == ' ')
        {
            // Get glyph description.
            const Glyph* glyph = info.font->GetGlyph(' ');
            assert(glyph != nullptr);

            // Advance drawing position.
            AdvanceBaseline(i, glyph);

            // Process a new word.
            wordProcessed = false;

            continue;
        }

        // Check if a word will fit in the current line.
        if(wordProcessed == false)
        {
            // Check if the next word will fit.
            if(info.size.x > 0.0f)
            {
                float wordSize = 0.0f;

                for(size_t j = i; j < textLength; ++j)
                {
                    FT_ULong wordCharacter = text[j];

                    // Space isn't a part of a ward.
                    if(wordCharacter == ' ')
                        break;

                    // We can't check if the first word will fit, it will cause problems.
                    assert(i != 0);

                    // Get glyph description.
                    const Glyph* glyph = info.font->GetGlyph(wordCharacter);
                    assert(glyph != nullptr);

                    // Get glyph kerning.
                    int kerning = info.font->GetKerning(text[j - i], wordCharacter);

                    // Check if the word will fit.
                    wordSize += glyph->advance.x + kerning;

                    if(baselinePosition.x - info.position.x + wordSize > info.size.x)
                    {
                        // Don't draw last space for debug base line.
                        if(info.debug)
                        {
                            const Glyph* glyphSpace = info.font->GetGlyph(' ');
                            assert(glyphSpace != nullptr);

                            baselinePosition.x -= glyphSpace->advance.x;
                        }

                        // Move to the next line.
                        MoveNextLine();

                        break;
                    }
                }
            }

            wordProcessed = true;
        }

        // Get glyph description.
        const Glyph* glyph = info.font->GetGlyph(character);
        assert(glyph != nullptr);

        // Apply glyph kerning.
        if(i != 0)
        {
            FT_ULong previous = text[i - 1];

            if(previous != '\n' && previous != ' ')
            {
                int kerning = info.font->GetKerning(previous, character);
                baselinePosition.x += kerning;
            }
        }

        // Calculate glyph rectangle.
        glm::vec4 rectangle;
        rectangle.x = (float)(baselinePosition.x + glyph->offset.x);
        rectangle.y = (float)(baselinePosition.y + glyph->offset.y);
        rectangle.w = rectangle.x + (float)glyph->size.x;
        rectangle.z = rectangle.y + (float)glyph->size.y;

        // Calculate glyph texture coordinates.
        glm::vec4 texture;
        texture.x = glyph->position.x * pixelSize.x;
        texture.y = glyph->position.y * pixelSize.y;
        texture.w = (glyph->position.x + glyph->size.x) * pixelSize.x;
        texture.z = (glyph->position.y + glyph->size.y) * pixelSize.y;

        // Create a character quad.
        Vertex quad[4] =
        {
            { glm::vec2(rectangle.x, rectangle.y), glm::vec2(texture.x, texture.y), info.color },
            { glm::vec2(rectangle.w, rectangle.y), glm::vec2(texture.w, texture.y), info.color },
            { glm::vec2(rectangle.w, rectangle.z), glm::vec2(texture.w, texture.z), info.color },
            { glm::vec2(rectangle.x, rectangle.z), glm::vec2(texture.x, texture.z), info.color },
        };

        // Draw debug glyph rectangle.
        if(info.debug)
        {
            AddDebugGlyphRectangle(glm::vec2(rectangle.x, rectangle.y), glm::vec2(glyph->size.x, glyph->size.y));
        }

        // Copy character quad to the vertex data.
        memcpy(&m_vertexData[charactersBuffered * 4], &quad[0], sizeof(Vertex) * 4);

        ++charactersBuffered;

        // Draw if we reached the buffer size or the last character.
        if(charactersBuffered == m_bufferSize)
        {
            DrawBufferedCharacters();
        }

        // Advance drawing position.
        AdvanceBaseline(i, glyph);
    }

    // Draw any remaining characters that were buffered.
    if(charactersBuffered != 0)
    {
        DrawBufferedCharacters();
    }

    // Unbind render states.
    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);

    // Draw text cursor.
    if(info.cursorIndex >= 0 && m_cursorTime < CursorBlinkTime * 0.5f)
    {
        ShapeRenderer::Line cursorLine;
        cursorLine.color = info.color;
        cursorLine.begin.x = cursorPosition.x;
        cursorLine.begin.y = cursorPosition.y + info.font->GetAscender();
        cursorLine.end.x = cursorPosition.x;
        cursorLine.end.y = cursorPosition.y + info.font->GetDescender();

        Context::ShapeRenderer().DrawLines(&cursorLine, 1, transform);
    }

    // Flush debug draw.
    if(info.debug)
    {
        // Draw glyph rectangles.
        Context::ShapeRenderer().DrawRectangles(&debugRectangles[0], debugRectangles.size(), transform);

        // Add last base line.
        AddDebugBaseLine(baselinePosition);

        // Draw all base lines.
        Context::ShapeRenderer().DrawLines(&debugLines[0], debugLines.size(), transform);

        // Draw bounding box.
        ShapeRenderer::Rectangle rectangle;
        rectangle.position.x = info.position.x;
        rectangle.position.y = baselinePosition.y + info.font->GetDescender();
        rectangle.size.x = info.size.x;
        rectangle.size.y = info.position.y - rectangle.position.y;
        rectangle.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        Context::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
    }
}
