#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"
#include "GameFactory.hpp"
#include "GameStages.hpp"

#include "InputState.hpp"

#include "EntitySystem.hpp"
#include "CollisionSystem.hpp"
#include "ScriptSystem.hpp"
#include "RenderSystem.hpp"

#include "HealthComponent.hpp"

#include "SpawnSystem.hpp"
#include "ProgressSystem.hpp"

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
    Game::ProgressSystem().SetNextStage(std::make_shared<AsteroidStage>());

    // Create bounds.
    Game::CreateBounds();

    // Create the player.
    m_playerEntity = Game::CreatePlayer();

    // Initialize the health bar.
    m_playerHealthBar.SetDrawingRectangle(glm::vec4(0.0f, 0.0f, 624.0f, 15.0f));
    m_playerHealthBar.SetForegroundColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetBackgroundColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    m_playerHealthBar.SetDecayColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

    HealthComponent* playerHealth = Game::HealthComponents().Lookup(m_playerEntity);
    if(playerHealth == nullptr)
    {
        Cleanup();
        return false;
    }

    m_playerHealthBar.SetMaximumValue((float)playerHealth->GetMaximumHealth());
    m_playerHealthBar.SetCurrentValue((float)playerHealth->GetCurrentHealth());

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

    // Update the health bar.
    HealthComponent* playerHealth = Game::HealthComponents().Lookup(m_playerEntity);

    if(playerHealth != nullptr)
    {
        m_playerHealthBar.SetMaximumValue((float)playerHealth->GetMaximumHealth());
        m_playerHealthBar.SetCurrentValue((float)playerHealth->GetCurrentHealth());
    }

    m_playerHealthBar.Update(timeDelta);
}

void GameFrame::Draw()
{
    // Render entities.
    Game::RenderSystem().Draw();

    // Draw the health bar.
    m_playerHealthBar.Draw(glm::vec2(200.0f, 10.0f), Game::RenderSystem().GetTransform());
}
