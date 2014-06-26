#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"

#include "MainContext.hpp"
#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleHistory.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

namespace
{
    // Console text rows.
    const int ConsoleSize = 16;
}

namespace Console
{
    // External console variables.
    extern ConsoleVariable windowWidth;
    extern ConsoleVariable windowHeight;
}

ConsoleFrame::ConsoleFrame() :
    m_input(""),
    m_cursorPosition(0),
    m_historyOutput(0),
    m_historyInput(0),
    m_open(false),
    m_initialized(false)
{
}

ConsoleFrame::~ConsoleFrame()
{
    Cleanup();
}

bool ConsoleFrame::Initialize()
{
    Cleanup();

    m_initialized = true;

    return true;
}

void ConsoleFrame::Cleanup()
{
    m_input.clear();
    m_cursorPosition = 0;

    m_historyOutput = 0;
    m_historyInput = 0;

    m_open = false;
    m_initialized = false;
}

void ConsoleFrame::Open()
{
    if(!m_initialized)
        return;

    m_open = true;
}

void ConsoleFrame::Close()
{
    if(!m_initialized)
        return;

    m_open = false;
}

bool ConsoleFrame::Process(const SDL_Event& event)
{
    if(!m_initialized)
        return false;

    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_GRAVE && event.key.repeat == 0)
        {
            if(!m_open)
            {
                // Open console frame.
                m_open = true;

                // Start text input.
                SDL_StartTextInput();

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
            else
            {
                // Close console frame.
                m_open = false;

                // End text input.
                SDL_StopTextInput();

                // Block the key that just closed 
                // the frame from being send further.
                return true;
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
        {
            if(m_open)
            {
                // Move cursor to the left.
                m_cursorPosition = std::max(0, m_cursorPosition - 1);

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
        {
            if(m_open)
            {
                // Move cursor to the right.
                int inputLength = utf8::distance(m_input.begin(), m_input.end());
                m_cursorPosition = std::min(m_cursorPosition + 1, inputLength);

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_UP)
        {
            if(m_open)
            {
                // Move to next history input entry.
                m_historyInput = std::min(m_historyInput + 1, Main::ConsoleHistory().GetInputSize());

                // Read history input.
                const char* input = Main::ConsoleHistory().GetInput(m_historyInput - 1);

                if(input != nullptr)
                {
                    // Set current input.
                    m_input = input;
                }
                else
                {
                    // Reset input.
                    m_input = "";
                }

                // Move cursor position to the end.
                int inputLength = utf8::distance(m_input.begin(), m_input.end());
                m_cursorPosition = inputLength;

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_DOWN)
        {
            if(m_open)
            {
                // Move to previous history input entry.
                m_historyInput = std::max(0, m_historyInput - 1);

                // Read history input.
                const char* input = Main::ConsoleHistory().GetInput(m_historyInput - 1);

                if(input != nullptr)
                {
                    // Set current input.
                    m_input = input;
                }
                else
                {
                    // Reset input.
                    m_input = "";
                }

                // Move cursor position to the end.
                int inputLength = utf8::distance(m_input.begin(), m_input.end());
                m_cursorPosition = inputLength;

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_PAGEUP)
        {
            if(m_open)
            {
                // Move output history up.
                m_historyOutput = std::min(m_historyOutput + 1, std::max(0, Main::ConsoleHistory().GetOutputSize() - ConsoleSize + 1));
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_PAGEDOWN)
        {
            if(m_open)
            {
                // Move output history down.
                m_historyOutput = std::max(0, m_historyOutput - 1);
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_HOME)
        {
            if(m_open)
            {
                // Move cursor to the beginning.
                m_cursorPosition = 0;

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_END)
        {
            if(m_open)
            {
                // Move cursor to the end.
                int inputLength = utf8::distance(m_input.begin(), m_input.end());
                m_cursorPosition = inputLength;

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_RETURN && event.key.repeat == 0)
        {
            if(m_open)
            {
                // Execute input.
                Main::ConsoleSystem().Execute(m_input);

                // Add to input history.
                Main::ConsoleHistory().WriteInput(m_input.c_str());

                // Reset input history position.
                m_historyInput = 0;

                // Reset output history position.
                m_historyOutput = 0;

                // Clear input.
                this->ClearInput();

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
        {
            if(m_open)
            {
                // Erase a character on the left of the cursor.
                if(m_cursorPosition > 0)
                {
                    // Determine part of the string we want to erase.
                    auto eraseBegin = m_input.begin();
                    utf8::unchecked::advance(eraseBegin, m_cursorPosition - 1);

                    auto eraseEnd = eraseBegin;
                    utf8::unchecked::advance(eraseEnd, 1);

                    // Erase a character.
                    m_input.erase(eraseBegin, eraseEnd);

                    // Move cursor back.
                    m_cursorPosition -= 1;

                    // Reset cursor blink.
                    Main::TextRenderer().ResetCursorBlink();
                }
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_DELETE)
        {
            if(m_open)
            {
                // Erase a character on the right of the cursor.
                int inputLength = utf8::distance(m_input.begin(), m_input.end());

                if(m_cursorPosition < inputLength)
                {
                    // Determine part of the string we want to erase.
                    auto eraseBegin = m_input.begin();
                    utf8::unchecked::advance(eraseBegin, m_cursorPosition);

                    auto eraseEnd = eraseBegin;
                    utf8::unchecked::advance(eraseEnd, 1);

                    // Erase a character.
                    m_input.erase(eraseBegin, eraseEnd);

                    // Reset cursor blink.
                    Main::TextRenderer().ResetCursorBlink();
                }
            }
        }
        else
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && event.key.repeat == 0)
        {
            if(m_open)
            {
                // Reset input history position.
                m_historyInput = 0;

                // Clear console input.
                this->ClearInput();

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        return m_open;

    case SDL_TEXTINPUT:
        if(m_open)
        {
            // Get the character string.
            const char* text = event.text.text;
            assert(text != nullptr);

            // Determine cursor position on the input buffer.
            auto insertBegin = m_input.begin();
            utf8::unchecked::advance(insertBegin, m_cursorPosition);

            // Insert entered characters at the cursor position.
            m_input.insert(insertBegin, text, text + strlen(text));
            m_cursorPosition += 1;

            // Reset cursor blink.
            Main::TextRenderer().ResetCursorBlink();
        }
        return m_open;

    case SDL_MOUSEBUTTONDOWN:
        // Block mouse button presses.
        return m_open;
    }

    return false;
}

void ConsoleFrame::Draw(const glm::mat4& transform, glm::vec2 targetSize)
{
    if(!m_initialized)
        return;

    if(m_open)
    {
        // Make sure output history is within range in case it gets cleared.
        m_historyOutput = std::min(m_historyOutput, std::max(0, Main::ConsoleHistory().GetOutputSize() - ConsoleSize + 1));

        // Calculate font sizes.
        const float fontSize = 16;
        const float fontSpacing = Main::DefaultFont().GetLineSpacing() * Main::DefaultFont().GetScaling(fontSize);

        // Calculate console metrics.
        float consoleExtra = 1.0f;
        float consoleSize = (float)ConsoleSize * fontSpacing;
        float consolePosition = targetSize.y - consoleSize;

        // Draw console background.
        {
            BasicRenderer::RectangleStyle style;
            style.drawMode = BasicRenderer::DrawMode::Fill;
            style.alphaBlend = true;

            BasicRenderer::Rectangle rectangle;
            rectangle.bottomleft = glm::vec2(0.0f, consolePosition - consoleExtra);
            rectangle.topright = targetSize;
            rectangle.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.85f);

            Main::BasicRenderer().DrawRectangles(style, &rectangle, 1, transform);
        }

        // Draw console output.
        for(int i = 0; i < ConsoleSize - 1; ++i)
        {
            const char* text = Main::ConsoleHistory().GetOutput(i + m_historyOutput);

            if(text == nullptr)
                break;

            TextDrawInfo info;
            info.font = &Main::DefaultFont();
            info.size = fontSize;
            info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            info.position.x = 5.0f;
            info.position.y = consolePosition + (i + 2) * fontSpacing;
            //info.area.x = windowWidth - 1.0f;
            info.area.x = 0.0f; // Text wrap doesn't work in console.
            info.area.y = 0.0f;

            Main::TextRenderer().Draw(info, text, transform);
        }

        // Draw console input.
        {
            std::string inputText = "> ";
            inputText += m_input;

            TextDrawInfo info;
            info.font = &Main::DefaultFont();
            info.size = fontSize;
            info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            info.position.x = 5.0f;
            info.position.y = consolePosition + fontSpacing;
            //info.area.x = windowWidth - 1.0f;
            info.area.x = 0.0f; // Text wrap doesn't work in console.
            info.area.y = 0.0f;
            info.cursorIndex = 2 + m_cursorPosition;

            Main::TextRenderer().Draw(info, inputText.c_str(), transform);
        }
    }
}

void ConsoleFrame::ClearInput()
{
    m_input.clear();
    m_cursorPosition = 0;
}
