#pragma once

#include "Precompiled.hpp"
#include "BaseFrame.hpp"

#include "InputState.hpp"

#include "EntitySystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

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
    void Draw();

private:
    InputState m_inputState;

    EntitySystem m_entitySystem;
    ScriptSystem m_scriptSystem;
    RenderSystem m_renderSystem;

    EntityHandle m_playerHandle;

    bool m_initialized;
};
