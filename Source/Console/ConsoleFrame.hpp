#pragma once

#include "Precompiled.hpp"

#include "Graphics/Font.hpp"

//
// Console Frame
//  Displays and lets user interact with the console system.
//

class ConsoleFrame
{
public:
    ConsoleFrame();
    ~ConsoleFrame();

    bool Initialize();
    void Cleanup();

    void Open();
    void Close();

    bool Process(const SDL_Event& event);
    void Draw(const glm::mat4& transform, glm::vec2 targetSize);

    bool IsOpen() const
    {
        return m_open;
    }

private:
    void ClearInput();

private:
    // Current input.
    std::string m_input;

    // Input cursor position.
    int m_cursorPosition;

    // History positions.
    int m_historyOutput;
    int m_historyInput;

    // Frame state.
    bool m_open;

    bool m_initialized;
};
