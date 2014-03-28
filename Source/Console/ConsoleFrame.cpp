#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleHistory.hpp"
#include "Graphics/ShapeRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "MainContext.hpp"

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

                // Reset global input state.
                Main::SendEvent(UserEvents::ResetInput);

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
                // Clear console input.
                this->ClearInput();

                // Reset cursor blink.
                Main::TextRenderer().ResetCursorBlink();
            }
        }
        break;

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
        break;

    case SDL_USEREVENT:
        if(event.user.code == UserEvents::ResetInput)
        {
            // Don't block input reset event we sent ourselves.
            return false;
        }
        break;
    }

    // Block all input events when console is open.
    return m_open;
}

void ConsoleFrame::Draw(const glm::mat4& transform, glm::vec2 targetSize)
{
    if(!m_initialized)
        return;

    if(m_open)
    {
        const float fontSize = 16;
        const float fontSpacing = Main::DefaultFont().GetLineSpacing() * Main::DefaultFont().GetScaling(fontSize);

        // Calculate console metrics.
        float consoleExtra = 1.0f;
        float consoleSize = (float)ConsoleSize * fontSpacing;
        float consolePosition = targetSize.y - consoleSize;

        // Draw console background.
        ShapeRenderer::Quad quad;
        quad.position.x = 0.0f;
        quad.position.y = consolePosition - consoleExtra;
        quad.size.x = targetSize.x;
        quad.size.y = consoleSize + consoleExtra;
        quad.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.85f);
        quad.texture = nullptr;

        Main::ShapeRenderer().DrawQuads(&quad, 1, transform);

        // Draw console text.
        for(int i = 0; i < ConsoleSize - 1; ++i)
        {
            const char* text = Main::ConsoleHistory().GetText(i);

            if(text == nullptr)
                break;

            TextDrawInfo info;
            info.font = &Main::DefaultFont();
            info.size = fontSize;
            info.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            info.position.x = 5.0f;
            info.position.y = consolePosition + (i + 2) * fontSpacing;
            //info.area.x = windowWidth - 1.0f;
            info.area.x = 0.0f; // Text wrap doesn't work in console.
            info.area.y = 0.0f;

            Main::TextRenderer().Draw(info, transform, text);
        }

        // Draw console input.
        {
            std::string inputText = "> ";
            inputText += m_input;

            TextDrawInfo info;
            info.font = &Main::DefaultFont();
            info.size = fontSize;
            info.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            info.position.x = 5.0f;
            info.position.y = consolePosition + fontSpacing;
            //info.area.x = windowWidth - 1.0f;
            info.area.x = 0.0f; // Text wrap doesn't work in console.
            info.area.y = 0.0f;
            info.cursorIndex = 2 + m_cursorPosition;

            Main::TextRenderer().Draw(info, transform, inputText.c_str());
        }
    }
}

void ConsoleFrame::ClearInput()
{
    m_input.clear();
    m_cursorPosition = 0;
}
