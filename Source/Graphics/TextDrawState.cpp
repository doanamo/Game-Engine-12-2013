#include "Precompiled.hpp"
#include "TextDrawState.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/TextRenderer.hpp"

TextDrawState::TextDrawState() :
    m_initialized(false),
    m_drawInfo(nullptr),
    m_fontScale(0.0f),
    m_textWrap(false),
    m_textBuffer(),
    m_textLength(0),
    m_textLine(0),
    m_textIterator(m_textBuffer.end()),
    m_characterIndex(-1),
    m_characterCurrent('\0'),
    m_characterPrevious('\0'),
    m_wordProcessed(true),
    m_glyph(nullptr),
    m_drawPosition(0.0f, 0.0f),
    m_cursorPosition(0.0f, 0.0f),
    m_cursorPresent(false),
    m_textArea(0.0f, 0.0f, 0.0f, 0.0f),
    m_boundingBox(0.0f, 0.0f, 0.0f, 0.0f),
    m_alignOffset(0.0f, 0.0f)
{
}

TextDrawState::~TextDrawState()
{
    Cleanup();
}

void TextDrawState::Cleanup()
{
    if(!m_initialized)
        return;

    m_drawInfo = nullptr;
    m_fontScale = 0.0f;
    m_textWrap = false;

    m_textBuffer.clear();
    m_textLength = 0;
    m_textLine = 0;

    m_textIterator = m_textBuffer.end();

    m_characterIndex = -1;
    m_characterCurrent = '\0';
    m_characterPrevious = '\0';

    m_wordProcessed = true;

    m_glyph = nullptr;

    m_drawPosition = glm::vec2(0.0f, 0.0f);

    m_cursorPosition = glm::vec2(0.0f, 0.0f);
    m_cursorPresent = false;

    m_textArea = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    m_boundingBox = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    m_alignOffset = glm::vec2(0.0f, 0.0f);

    m_initialized = false;
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

    // Perform UTF8 validation on the text string and copy it to a buffer.
    const char InvalidCharacter = '?';

    try
    {
        utf8::replace_invalid(text, text + strlen(text), std::back_inserter(m_textBuffer), InvalidCharacter);
    }
    catch(const utf8::not_enough_room&)
    {
        // Workaround for UTF8 library bug.
        // Function utf8::replace_invalid() sometimes throws this exception if
        // the last UTF8 character is invalid, instead of just replacing it.
        // Example string that triggers it: "Helloæ" (encoded in ANSI, not UTF8)
        m_textBuffer.push_back(InvalidCharacter);
    }

    // Calculate text length.
    m_textLength = utf8::distance(m_textBuffer.begin(), m_textBuffer.end());

    // Set the text iterators.
    m_textIterator = m_textBuffer.begin();

    // Check if we want to wrap the text.
    if(info.area.x > 0.0f)
        m_textWrap = true;

    // Set initial drawing position.
    m_drawPosition.x = info.position.x;
    m_drawPosition.y = info.position.y - info.font->GetAscender() * m_fontScale;

    // Set initial cursor position.
    if(m_drawInfo->cursorIndex >= 0 && m_drawInfo->cursorIndex <= m_textLength)
    {
        m_cursorPosition = m_drawPosition;
        m_cursorPresent = true;
    }

    // Set base initialized state.
    m_initialized = true;

    // Calculate text metrics.
    this->CalculateMetrics();

    return true;
}

// Processes the next character.
// Returns true if processed character should be drawn.
// Returns false if processed character shouldn't be drawn (in case of whitespaces).
bool TextDrawState::ProcessNext()
{
    if(!m_initialized)
        return false;

    // Check if we are done processing the text.
    if(this->IsDone())
        return false;

    // Advance position from previously drawn glyph.
    if(m_characterIndex >= 0 && m_glyph != nullptr)
    {
        AdvancePosition(m_glyph);
    }

    // Reset current glyph pointer.
    m_glyph = nullptr;

    // Save the previously processed character.
    m_characterPrevious = m_characterCurrent;

    // Get the next UTF-8 encoded character.
    std::string::iterator currentIterator = m_textIterator;
    m_characterCurrent = utf8::next(m_textIterator, m_textBuffer.end());

    // Increment the character index.
    ++m_characterIndex;

    // Handle special tokens.
    if(m_characterCurrent == '\n')
    {
        // Move to the next line.
        if(m_characterIndex != m_textLength - 1)
        {
            MoveNextLine();
        }

        // Skip this character entirely.
        return false;
    }
    else
    if(m_characterCurrent == ' ')
    {
        // Process a new word.
        m_wordProcessed = false;
    }
        
    // Process next word.
    if(m_characterCurrent != ' ' && !m_wordProcessed)
    {
        // Check if a word will fit in the current line.
        if(m_textWrap)
        {
            float wordSize = 0.0f;

            FT_ULong wordCharacterPrevious = ' ';
            std::string::iterator wordCharacterIterator = currentIterator;

            while(wordCharacterIterator != m_textBuffer.end())
            {
                // Get the next word character.
                FT_ULong wordCharacterCurrent = utf8::next(wordCharacterIterator, m_textBuffer.end());

                // Check if we reached the end of a word.
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
    m_glyph = m_drawInfo->font->GetGlyph(m_characterCurrent);
    assert(m_glyph != nullptr);

    // Apply glyph kerning.
    if(m_characterIndex != 0)
    {
        float kerning = m_drawInfo->font->GetKerning(m_characterPrevious, m_characterCurrent);
        m_drawPosition.x += kerning * m_fontScale;
    }

    // Check if it's the cursor position.
    if(m_drawInfo->cursorIndex == m_characterIndex + 1)
    {
        m_cursorPosition.x = m_drawPosition.x + m_glyph->advance.x * m_fontScale;
        m_cursorPosition.y = m_drawPosition.y + m_glyph->advance.y * m_fontScale;
    }

    // Draw this character.
    return true;
}

bool TextDrawState::IsDone() const
{
    return m_textIterator == m_textBuffer.end();
}

void TextDrawState::Reset()
{
    if(!m_initialized)
        return;

    // Reset character iterators.
    m_textIterator = m_textBuffer.begin();

    m_characterIndex = -1;
    m_characterCurrent = '\0';
    m_characterPrevious = '\0';

    // Reset text state.
    m_wordProcessed = true;

    // Reset current glyph.
    m_glyph = nullptr;

    // Reset drawing position.
    m_drawPosition.x = m_drawInfo->position.x;
    m_drawPosition.y = m_drawInfo->position.y - m_drawInfo->font->GetAscender() * m_fontScale;

    // Reset cursor position.
    if(m_drawInfo->cursorIndex >= 0 && m_drawInfo->cursorIndex <= m_textLength)
    {
        m_cursorPosition = m_drawPosition;
        m_cursorPresent = true;
    }
}

void TextDrawState::CalculateMetrics()
{
    assert(m_initialized);

    // Set initial bounding box.
    m_boundingBox.x = m_boundingBox.y = std::numeric_limits<float>::max();
    m_boundingBox.z = m_boundingBox.w = std::numeric_limits<float>::min();

    // Process all characters.
    while(!this->IsDone())
    {
        // Process next character.
        if(!this->ProcessNext())
            continue;

        // Calculate current bounding box.
        m_boundingBox.x = std::min(m_boundingBox.x, m_drawPosition.x + m_glyph->offset.x * m_fontScale);
        m_boundingBox.y = std::min(m_boundingBox.y, m_drawPosition.y + m_glyph->offset.y * m_fontScale);
        m_boundingBox.z = std::max(m_boundingBox.z, m_drawPosition.x + (m_glyph->offset.x + m_glyph->size.x) * m_fontScale);
        m_boundingBox.w = std::max(m_boundingBox.w, m_drawPosition.y + (m_glyph->offset.y + m_glyph->size.y) * m_fontScale);

        // Calculate current text area.
        m_textArea.x = m_drawInfo->position.x;
        m_textArea.y = m_drawPosition.y + m_drawInfo->font->GetDescender() * m_fontScale;

        if(m_textWrap)
        {
            m_textArea.z = m_drawInfo->position.x + m_drawInfo->area.x;
        }
        else
        {
            m_textArea.z = std::max(m_textArea.z, m_drawPosition.x + m_glyph->advance.x * m_fontScale);
        }

        m_textArea.w = m_drawInfo->position.y;
    }

    // Calculate text align offset.
    switch(m_drawInfo->align)
    {
    case TextDrawAlign::TopLeft:
        // Default align.
        break;

    case TextDrawAlign::TopRight:
        m_alignOffset.x -= (m_textArea.z - m_textArea.x);
        break;

    case TextDrawAlign::BottomLeft:
        m_alignOffset.y += (m_textArea.w - m_textArea.y);
        break;

    case TextDrawAlign::BottomRight:
        m_alignOffset.x -= (m_textArea.z - m_textArea.x);
        m_alignOffset.y += (m_textArea.w - m_textArea.y);
        break;

    case TextDrawAlign::Centered:
        m_alignOffset.x -= (m_textArea.z - m_textArea.x) * 0.5f;
        m_alignOffset.y += (m_textArea.w - m_textArea.y) * 0.5f;
        break;
    }

    // Reset the draw state.
    this->Reset();
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

    // Advance position for the next glyph.
    m_drawPosition.x += glyph->advance.x * m_fontScale;
    m_drawPosition.y += glyph->advance.y * m_fontScale;
}
