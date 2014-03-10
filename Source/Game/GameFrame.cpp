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

#include "HealthComponent.hpp"

void SpawnFunction(const glm::vec2& position)
{
    Game::CreateEnemy(position);
}

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

    // Initialize the spawn system.
    if(!m_spawnSystem.Initialize())
        return false;

    m_spawnSystem.SetSpawnArea(glm::vec4(1024.0f + 100.0f, 50.0f, 1024.0f + 100.0f, 526.0f));
    m_spawnSystem.AddSpawn(&SpawnFunction, 0.5f, 1.0f);

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
    // Update spawn system.
    m_spawnSystem.Update(timeDelta);

    // Process entity commands.
    Game::EntitySystem().ProcessCommands();

    // Update collision system.
    Game::CollisionSystem().Update();

    // Update script system.
    Game::ScriptSystem().Update(timeDelta);

    // Update render system.
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
