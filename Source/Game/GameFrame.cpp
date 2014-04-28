#include "Precompiled.hpp"
#include "GameFrame.hpp"

#include "MainContext.hpp"
#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/GameStages.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/LoseFrame.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputState.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Render/RenderSystem.hpp"
#include "Game/Interface/InterfaceSystem.hpp"
#include "Game/Spawn/SpawnSystem.hpp"
#include "Game/Progress/ProgressSystem.hpp"

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

    // Setup the spawn system.
    GameState::SpawnSystem().SetSpawnArea(glm::vec4(1024.0f + 100.0f, 50.0f, 1024.0f + 100.0f, 526.0f));

    // Setup the progress system.
    GameState::ProgressSystem().SetNextStage(std::make_shared<EnemyStage>());

    // Create bounds.
    GameFactory::CreateBounds();

    // Create the player.
    GameFactory::CreatePlayer();

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
            GameContext::FrameState().ChangeState(&GameContext::MenuFrame());
        }
        break;
    }

    // Process input events.
    GameState::InputState().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Update the progress system.
    GameState::ProgressSystem().Update(timeDelta);

    // Update the spawn system.
    GameState::SpawnSystem().Update(timeDelta);

    // Process entity commands.
    GameState::EntitySystem().ProcessCommands();

    // Update the collision system.
    GameState::CollisionSystem().Update(timeDelta);

    // Update the script system.
    GameState::ScriptSystem().Update(timeDelta);

    // Update the render system.
    GameState::RenderSystem().Update();

    // Update the interface system.
    GameState::InterfaceSystem().Update(timeDelta);

    // Check if player is dead.
    if(!m_playerDead)
    {
        EntityHandle player = GameState::IdentitySystem().GetEntityByName("Player");

        if(!GameState::EntitySystem().IsHandleValid(player))
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
            GameContext::GameFrame().Cleanup();
            GameContext::FrameState().ChangeState(&GameContext::LoseFrame());
        }
    }
}

void GameFrame::Draw()
{
    // Draw the world.
    GameState::RenderSystem().Draw();

    // Draw the interface.
    GameState::InterfaceSystem().Draw();
}
