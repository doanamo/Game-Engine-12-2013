#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"

#include "Transform.hpp"
#include "Render.hpp"

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

    // Initialize the render system.
    if(!m_renderSystem.Initialize())
        return false;

    m_entitySystem.RegisterSubsystem(&m_renderSystem);

    // Create entities.
    {
        // Create an entity.
        Entity* entity = m_entitySystem.CreateEntity();
        m_playerHandle = entity->GetHandle();

        // Create components.
        std::unique_ptr<Transform> transform(new Transform());
        transform->SetPosition(glm::vec2(50.0f, 275.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);
        entity->InsertComponent(transform);

        std::unique_ptr<Render> render(new Render());
        render->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        entity->InsertComponent(render);
    }

    for(int i = 0; i < 4; ++i)
    {
        // Create an entity.
        Entity* entity = m_entitySystem.CreateEntity();

        // Create components.
        std::unique_ptr<Transform> transform(new Transform());
        transform->SetPosition(glm::vec2(900.0f, 100.0f + i * 100.0f));
        transform->SetScale(glm::vec2(1.0f, 1.0f));
        transform->SetRotation(0.0f);
        entity->InsertComponent(transform);

        std::unique_ptr<Render> render(new Render());
        render->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        entity->InsertComponent(render);
    }

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    m_playerHandle = EntityHandle();

    m_entitySystem.Cleanup();
    m_renderSystem.Cleanup();

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
        return true;
    }

    return false;
}

void GameFrame::Update(float dt)
{
    m_entitySystem.Update(dt);
}

void GameFrame::Draw()
{
    // Clear the screen.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render entities.
    m_renderSystem.Draw();
}
