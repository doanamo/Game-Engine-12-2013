#pragma once

#include "Precompiled.hpp"

#include "Graphics/Font.hpp"

//
// Console Frame
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
    std::string m_input;
    int m_cursorPosition;

    bool m_open;
    bool m_initialized;
};
