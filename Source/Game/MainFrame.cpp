#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainContext.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
    Cleanup();
}

bool MainFrame::Initialize()
{
    Cleanup();

    if(!m_font.Load(Main::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 64, 512, 512))
    {
        Cleanup();
        return false;
    }

    return true;
}

void MainFrame::Cleanup()
{
    m_font.Cleanup();
}

bool MainFrame::Process(const SDL_Event& event)
{
    return false;
}

void MainFrame::Update(float dt)
{
}

void MainFrame::Draw()
{
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    glm::mat4 transform = glm::ortho(0.0f, windowWidth * 0.25f, 0.0f, windowHeight * 0.25f);

    // Clear the back buffer.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);

    // Draw some text.
    TextRenderer::DrawInfo drawInfo;
    drawInfo.font = &m_font;
    drawInfo.position = glm::vec2(20.0f, 155.0f);

    Main::TextRenderer().Draw(drawInfo, transform, "Hello\n   world!");
}
