#include "Precompiled.hpp"
#include "GameFrame.hpp"

#include "MainContext.hpp"
#include "Game/GameContext.hpp"
#include "Game/GameFactory.hpp"
#include "Game/GameStages.hpp"
#include "Game/MenuFrame.hpp"
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
    m_initialized(false)
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

    // Initialize the game context.
    if(!Game::Initialize())
        return false;

    // Setup the spawn system.
    Game::SpawnSystem().SetSpawnArea(glm::vec4(1024.0f + 100.0f, 50.0f, 1024.0f + 100.0f, 526.0f));

    // Setup the progress system.
    Game::ProgressSystem().SetNextStage(std::make_shared<EnemyStage>());

    // Create bounds.
    Game::CreateBounds();

    // Create the player.
    Game::CreatePlayer();

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    Game::Cleanup();

    m_initialized = false;
}

bool GameFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            Game::FrameState().ChangeState(&Game::MenuFrame());
        }
        break;
    }

    // Process input events.
    Game::InputState().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Update the progress system.
    Game::ProgressSystem().Update(timeDelta);

    // Update the spawn system.
    Game::SpawnSystem().Update(timeDelta);

    // Process entity commands.
    Game::EntitySystem().ProcessCommands();

    // Update the collision system.
    Game::CollisionSystem().Update(timeDelta);

    // Update the script system.
    Game::ScriptSystem().Update(timeDelta);

    // Update the render system.
    Game::RenderSystem().Update();

    // Update the interface system.
    Game::InterfaceSystem().Update(timeDelta);
}

void GameFrame::Draw()
{
    // Draw the world.
    Game::RenderSystem().Draw();

    // Draw the interface.
    Game::InterfaceSystem().Draw();
}
