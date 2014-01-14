#include "Precompiled.hpp"
#include "GameFrame.hpp"
#include "MenuFrame.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Context.hpp"
#include "Game.hpp"

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

    // Load font.
    if(!m_font.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 64, 512, 512))
        return false;

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    m_font.Cleanup();

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
}

void GameFrame::Draw(const glm::mat4& transform)
{
    // Get window size.
    int windowWidth = Console::windowWidth.GetInteger();
    int windowHeight = Console::windowHeight.GetInteger();

    // Draw menu title.
    {
        TextRenderer::DrawInfo info;
        info.font = &m_font;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 175.0f;
        info.position.y = (windowHeight + m_font.GetLineSpacing()) * 0.5f;

        Context::TextRenderer().Draw(info, transform, "Insert gameplay here :P");
    }
}
