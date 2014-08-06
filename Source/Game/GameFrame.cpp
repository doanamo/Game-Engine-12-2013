#include "Precompiled.hpp"
#include "GameFrame.hpp"

#include "MainGlobal.hpp"
#include "Scripting/LuaEngine.hpp"
#include "Scripting/LuaMath.hpp"
#include "Scripting/LuaGame.hpp"
#include "Game/GameGlobal.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
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

namespace
{
    void SpawnFunction(const glm::vec2& position)
    {
        GameFactory::CreateEnemy(position);
    }
}

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

    // Setup emergency cleanup.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!m_initialized)
        {
            Cleanup();
        }
    });

    // Initialize the game state.
    if(!GameState::Initialize())
        return false;

    // Setup scripting environment.
    Main::GetLuaEngine().SetPackagePath(Main::GetWorkingDir() + "Data/");

    if(!BindLuaMath(Main::GetLuaEngine()))
        return false;

    if(!BindLuaGame(Main::GetLuaEngine(), GameState::GetServices()))
        return false;

    // Load the main script.
    if(!Main::GetLuaEngine().Load("Data/Main.lua"))
        return false;

    // Call the initialization function.
    Main::GetLuaEngine().Call("Initialize");

    // Create bounds.
    GameFactory::CreateBounds();

    // Setup the spawn system.
    GameState::GetSpawnSystem().SetSpawnArea(glm::vec4(1024.0f + 100.0f, 50.0f, 1024.0f + 100.0f, 526.0f));
    GameState::GetSpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    GameState::Cleanup();

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
