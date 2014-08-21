#include "Precompiled.hpp"
#include "GameFrame.hpp"

#include "MainGlobal.hpp"
#include "Scripting/LuaEngine.hpp"
#include "Game/GameGlobal.hpp"
#include "Game/GameState.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/LoseFrame.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputSystem.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Render/RenderSystem.hpp"
#include "Game/Interface/InterfaceSystem.hpp"
#include "Game/Spawn/SpawnSystem.hpp"

GameFrame::GameFrame() :
    m_initialized(false),
    m_playerDead(false),
    m_deadTimer(0.0f)
{
}

GameFrame::~GameFrame()
{
    Cleanup();
}

bool GameFrame::Initialize()
{
    Cleanup();

    // Setup scope guard.
    SCOPE_GUARD_IF(!m_initialized, Cleanup());

    // Initialize the game state.
    if(!m_gameState.Initialize())
        return false;

    // Success!
    return m_initialized = true;
}

void GameFrame::Cleanup()
{
    m_initialized = false;

    m_gameState.Cleanup();

    m_playerDead = false;
    m_deadTimer = 0.0f;
}

bool GameFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            // Prevent from quitting to the main menu if player is dead.
            if(m_playerDead)
                break;

            // Change to main menu state.
            GameContext::GetFrameState().ChangeState(&GameContext::GetMenuFrame());
        }
        break;
    }

    // Process events by game state.
    m_gameState.Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Update the game state.
    m_gameState.Update(timeDelta);

    // Check if player is dead.
    if(!m_playerDead)
    {
        EntityHandle player = m_gameState.GetIdentitySystem().GetEntityByName("Player");

        if(!m_gameState.GetEntitySystem().IsHandleValid(player))
        {
            // Set the dead state.
            m_playerDead = true;
        }
    }
    else
    {
        // Increment the dead timer.
        m_deadTimer += timeDelta;

        // Change to lose frame (game over screen).
        if(m_deadTimer > 4.0f)
        {
            GameContext::GetGameFrame().Cleanup();
            GameContext::GetFrameState().ChangeState(&GameContext::GetLoseFrame());
        }
    }
}

void GameFrame::Draw()
{
    // Draw the game state.
    m_gameState.Draw();
}
