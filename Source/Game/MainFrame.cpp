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

    if(!m_font.Load(Main::WorkingDir() + "Data/Fonts/SourceSansPro.ttf"))
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

    float scale = 0.25f;

    glm::mat4 transform = glm::ortho(0.0f, windowWidth * scale, 0.0f, windowHeight * scale);

    // Clear the back buffer.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);

    // Draw some text.
    TextRenderer::DrawInfo drawInfo;
    drawInfo.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    drawInfo.font = &m_font;
    drawInfo.position = glm::vec2(10.0f, 450.0f);
    drawInfo.debug = true;

    std::string ascii;
    int breakCounter = 0;

    for(char c = 33; c < 127; ++c)
    {
        ascii.push_back(c);

        if(++breakCounter == 20)
        {
            ascii.push_back('\n');
            breakCounter = 0;
        }
    }

    Main::TextRenderer().Draw(drawInfo, transform, ascii.c_str());

    // Those character will break the output: óœ³ (not utf8 valid encoding, repalce with ?).
}
