#pragma once

#include "Precompiled.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/TextRenderer.hpp"

//
// Menu Frame
//

class MenuFrame
{
public:
    MenuFrame();
    ~MenuFrame();

    bool Initialize();
    void Cleanup();

    void Process(const SDL_Event& event);
    void Update(float dt);
    void Draw(const glm::mat4& projection);

private:
    Font m_fontTitle;
    Font m_fontOption;

    static const int       OptionCount = 5;
    const char*            m_optionText[OptionCount];
    TextRenderer::DrawInfo m_optionDrawInfo[OptionCount];
    glm::vec4              m_optionBoundingBox[OptionCount];

    bool m_initialized;
};
