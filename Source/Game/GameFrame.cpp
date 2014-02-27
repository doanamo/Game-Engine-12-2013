#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "InputState.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

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

    // Create bounds.
    Game::CreateBounds();

    // Create the player.
    Game::CreatePlayer();

    // Create spawners.
    for(int y = 0; y < 8; ++y)
    {
        Game::CreateSpawner(glm::vec2(1024.0f + 100.0f, 45.0f + y * 70.0f));
    }

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    Game::EntitySystem().DestroyAllEntities();

    m_initialized = false;
}

bool GameFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            Game::StateMachine().ChangeState(&Game::MenuFrame());
        }
        break;
    }

    // Process input events.
    Game::InputState().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Process entity commands.
    Game::EntitySystem().ProcessCommands();

    // Update collision system.
    Game::CollisionSystem().Update();

    // Update script system.
    Game::ScriptSystem().Update(timeDelta);

    // Update render system.
    Game::RenderSystem().Update();
}

void GameFrame::Draw()
{
    // Render entities.
    Game::RenderSystem().Draw();
}
