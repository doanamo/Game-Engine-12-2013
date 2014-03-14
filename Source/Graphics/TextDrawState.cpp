#include "Precompiled.hpp"
#include "TextDrawState.hpp"

#include "Font.hpp"
#include "TextRenderer.hpp"

TextDrawState::TextDrawState() :
    m_drawInfo(nullptr),
    m_fontScale(0.0f),
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

TextDrawState::~TextDrawState()
{
    Cleanup();
}

bool TextDrawState::Initialize(const TextDrawInfo& info, const char* text)
{
    Cleanup();

    // Validate arguments.
    if(info.font == nullptr)
        return false;

    if(info.size <= 0.0f)
        return false;

    if(info.area.x < 0.0f)
        return false;

    if(info.area.y < 0.0f)
        return false;

    if(text == nullptr)
        return false;

    m_drawInfo = &info;

    // Calculate font scale.
    m_fontScale = info.font->GetScaling(info.size);

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
    if(info.area.x > 0.0f)
        m_wordWrap = true;

    // Set initial drawing position.
    m_drawPosition.x = info.position.x;
    m_drawPosition.y = info.position.y - info.font->GetAscender() * m_fontScale;

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

void TextDrawState::Cleanup()
{
    if(!m_initialized)
        return;

    m_drawInfo = nullptr;

    m_fontScale = 0.0f;

    m_textLength = 0;
    m_textLine = 0;
    m_textIterator = nullptr;
    m_textEnd = nullptr;

    m_characterIndex = -1;
    m_characterCurrent = '\0';
    m_characterPrevious = '\0';

    m_wordProcessed = true;
    m_wordWrap = false;

    m_drawPosition = glm::vec2(0.0f, 0.0f);
    m_drawArea = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    m_glyph = nullptr;

    m_cursorPosition = glm::vec2(0.0f, 0.0f);
    m_cursorPresent = false;

    m_boundingBox = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    m_initialized = false;
}

bool TextDrawState::ProcessNext()
{
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
                float kerning = m_drawInfo->font->GetKerning(wordCharacterPrevious, wordCharacterCurrent);

                // Check if the word will fit.
                wordSize += (glyph->advance.x + kerning) * m_fontScale;

                if(m_drawPosition.x - m_drawInfo->position.x + wordSize > m_drawInfo->area.x)
                {
                    // Don't draw last space for debug base line.
                    if(m_drawInfo->debug)
                    {
                        const Glyph* glyphSpace = m_drawInfo->font->GetGlyph(' ');
                        assert(glyphSpace != nullptr);

                        m_drawPosition.x -= glyphSpace->advance.x * m_fontScale;
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
        float kerning = m_drawInfo->font->GetKerning(m_characterPrevious, m_characterCurrent);
        m_drawPosition.x += kerning * m_fontScale;
    }

    // Check if it's the cursor position.
    if(m_drawInfo->cursorIndex == m_characterIndex + 1)
    {
        m_cursorPosition.x = m_drawPosition.x + glyph->advance.x * m_fontScale;
        m_cursorPosition.y = m_drawPosition.y + glyph->advance.y * m_fontScale;
    }

    // Set initial bounding box.
    if(m_characterIndex == 0)
    {
        m_boundingBox.x = m_boundingBox.y = std::numeric_limits<float>::max();
        m_boundingBox.z = m_boundingBox.w = std::numeric_limits<float>::min();
    }

    // Calculate current bounding box.
    m_boundingBox.x = std::min(m_boundingBox.x, m_drawPosition.x + glyph->offset.x * m_fontScale);
    m_boundingBox.y = std::min(m_boundingBox.y, m_drawPosition.y + glyph->offset.y * m_fontScale);
    m_boundingBox.z = std::max(m_boundingBox.z, m_drawPosition.x + (glyph->offset.x + glyph->size.x) * m_fontScale);
    m_boundingBox.w = std::max(m_boundingBox.w, m_drawPosition.y + (glyph->offset.y + glyph->size.y) * m_fontScale);

    // Calculate current draw area.
    m_drawArea.x = m_drawInfo->position.x;
    m_drawArea.y = m_drawPosition.y + m_drawInfo->font->GetDescender() * m_fontScale;

    if(m_wordWrap)
    {
        m_drawArea.z = m_drawInfo->position.x + m_drawInfo->area.x;
    }
    else
    {
        m_drawArea.z = std::max(m_drawArea.z, m_drawPosition.x + glyph->advance.x * m_fontScale);
        //m_drawArea.z = m_boundingBox.z;
    }

    m_drawArea.w = m_drawInfo->position.y;

    return false;
}

bool TextDrawState::IsDone() const
{
    return m_textIterator == m_textEnd;
}

void TextDrawState::MoveNextLine()
{
    if(!m_initialized)
        return;

    // Move to the next line.
    m_drawPosition.x = m_drawInfo->position.x;
    m_drawPosition.y -= m_drawInfo->font->GetLineSpacing() * m_fontScale;

    // Increase line counter.
    ++m_textLine;
}

void TextDrawState::AdvancePosition(const Glyph* glyph)
{
    assert(glyph != nullptr);

    if(!m_initialized)
        return;

    if(glyph == nullptr)
        return;

    // Advance position for next glyph.
    m_drawPosition.x += glyph->advance.x * m_fontScale;
    m_drawPosition.y += glyph->advance.y * m_fontScale;
}
