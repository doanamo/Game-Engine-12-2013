#pragma once

#include "Precompiled.hpp"

// Forward declarations.
struct TextDrawInfo;
struct Glyph;

//
// Text Draw State
//  Processes text for rendering.
//

class TextDrawState
{
public:
    TextDrawState();
    ~TextDrawState();

    bool Initialize(const TextDrawInfo& info, const char* text);
    void Cleanup();

    bool ProcessNext();
    bool IsDone() const;
    void Reset();

private:
    void CalculateMetrics();

    void MoveNextLine();
    void AdvancePosition(const Glyph* glyph);

public:
    float GetFontScale() const
    {
        return m_fontScale;
    }

    bool IsTextWrapEnabled() const
    {
        return m_textWrap;
    }

    const std::string& GetProcessedString() const
    {
        return m_textBuffer;
    }

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

    const Glyph* GetCurrentGlyph() const
    {
        return m_glyph;
    }

    const glm::vec2& GetCursorPosition() const
    {
        return m_cursorPosition;
    }

    const glm::vec4& GetTextArea() const
    {
        return m_textArea;
    }

    const glm::vec4& GetBoundingBox() const
    {
        return m_boundingBox;
    }

    const glm::vec2& GetAlignOffset() const
    {
        return m_alignOffset;
    }

    bool IsCursorPresent() const
    {
        return m_cursorPresent;
    }

private:
    bool m_initialized;

    //
    // Draw Info
    //

    // Text info.
    const TextDrawInfo* m_drawInfo;

    float m_fontScale;
    bool  m_textWrap;

    // Text buffer.
    std::string m_textBuffer;
    int m_textLength;
    int m_textLine;

    //
    // Draw State
    //

    // Text iterator.
    std::string::iterator m_textIterator;

    // Current character.
    int      m_characterIndex;
    FT_ULong m_characterCurrent;
    FT_ULong m_characterPrevious;

    // Text wrap.
    bool m_wordProcessed;

    // Current text glyph.
    const Glyph* m_glyph;

    // Current draw position.
    glm::vec2 m_drawPosition;

    // Text cursor.
    glm::vec2 m_cursorPosition;
    bool      m_cursorPresent;

    //
    // Draw Metrics
    //

    // Text metrics.
    glm::vec4 m_textArea;
    glm::vec4 m_boundingBox;

    // Text align offset.
    glm::vec2 m_alignOffset;
};
