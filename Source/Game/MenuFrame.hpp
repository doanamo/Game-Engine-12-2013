#pragma once

#include "Precompiled.hpp"
#include "Graphics/Font.hpp"

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

    bool m_initialized;
};
