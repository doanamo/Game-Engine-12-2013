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

    float scale = 1.0f;

    glm::mat4 transform = glm::ortho(0.0f, windowWidth * scale, 0.0f, windowHeight * scale);

    // Clear the back buffer.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);

    // Draw some text.
    TextDrawInfo drawInfo;
    drawInfo.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    drawInfo.font = &m_font;
    drawInfo.size = 512;
    drawInfo.position = glm::vec2(50.0f, 800.0f * scale);
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

    //Main::TextRenderer().Draw(drawInfo, transform, ascii.c_str());
    Main::TextRenderer().Draw(drawInfo, transform, "X/y67");

    // Those character will break the output: � (not utf8 valid encoding, repalce with ?).
}
