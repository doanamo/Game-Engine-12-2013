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
    if(!GameState::Initialize())
        return false;

    // Success!
    return m_initialized = true;
}

void GameFrame::Cleanup()
{
    if(m_initialized)
    {
        GameState::Cleanup();
    }

    m_initialized = false;

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

    // Process input events.
    GameState::GetInputSystem().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Collect scripting garbage.
    GameState::GetLuaEngine().CollectGarbage(0.02f);

    // Update the spawn system.
    GameState::GetSpawnSystem().Update(timeDelta);

    // Process entity commands.
    GameState::GetEntitySystem().ProcessCommands();

    // Update the collision system.
    GameState::GetCollisionSystem().Update(timeDelta);

    // Update the script system.
    GameState::GetScriptSystem().Update(timeDelta);

    // Update the render system.
    GameState::GetRenderSystem().Update();

    // Update the interface system.
    GameState::GetInterfaceSystem().Update(timeDelta);

    // Check if player is dead.
    if(!m_playerDead)
    {
        EntityHandle player = GameState::GetIdentitySystem().GetEntityByName("Player");

        if(!GameState::GetEntitySystem().IsHandleValid(player))
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
    // Draw the world.
    GameState::GetRenderSystem().Draw();

    // Draw the interface.
    GameState::GetInterfaceSystem().Draw();
}
