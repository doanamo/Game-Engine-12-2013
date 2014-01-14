#pragma once

#include "Precompiled.hpp"
#include "Graphics/Font.hpp"
#include "BaseFrame.hpp"

//
// Game Frame
//

class GameFrame : public BaseFrame
{
public:
    GameFrame();
    ~GameFrame();

    bool Initialize();
    void Cleanup();

    bool Process(const SDL_Event& event);
    void Update(float dt);
    void Draw(const glm::mat4& transform);

private:
    Font m_font;

    bool m_initialized;
};
