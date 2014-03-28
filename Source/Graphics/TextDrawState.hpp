#pragma once

#include "Precompiled.hpp"

// Forward declarations.
struct TextDrawInfo;
struct Glyph;

//
// Text Draw State
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

private:
    void MoveNextLine();
    void AdvancePosition(const Glyph* glyph);

public:
    float GetFontScale() const
    {
        return m_fontScale;
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
    const TextDrawInfo* m_drawInfo;

    // Text font.
    float m_fontScale;

    // Text buffer.
    std::string           m_textBuffer;
    std::string::iterator m_textIterator;

    int m_textLength;
    int m_textLine;

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
