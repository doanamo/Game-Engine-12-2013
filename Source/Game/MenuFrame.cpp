#include "Precompiled.hpp"
#include "MenuFrame.hpp"
#include "Graphics/TextRenderer.hpp"

MenuFrame::MenuFrame() :
    m_initialized(false)
{
}

MenuFrame::~MenuFrame()
{
    Cleanup();
}

bool MenuFrame::Initialize()
{
    Cleanup();

    // Setup emergency cleanup.
    SCOPE_GUARD
    (
        if(!m_initialized)
        {
            Cleanup();
        }
    );

    // Load fonts.
    if(!m_fontTitle.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 128, 512, 512))
        return false;

    if(!m_fontOption.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 32, 512, 512))
        return false;

    // Success!
    m_initialized = true;

    return true;
}

void MenuFrame::Cleanup()
{
    m_fontTitle.Cleanup();
    m_fontOption.Cleanup();

    m_initialized = false;
}

void MenuFrame::Process(const SDL_Event& event)
{
}

void MenuFrame::Update(float dt)
{
}

void MenuFrame::Draw(const glm::mat4& projection)
{
    int windowWidth = Console::windowWidth.GetInteger();
    int windowHeight = Console::windowHeight.GetInteger();

    // Draw the title.
    {
        TextRenderer::DrawInfo info;
        info.font = &m_fontTitle;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 50.0f;
        info.position.y = windowHeight - 10.0f;
        //info.debug = true;

        Context::TextRenderer().Draw(info, projection, "Awesome Game");
    }

    // Draw the options.
    {
        TextRenderer::DrawInfo info;
        info.font = &m_fontOption;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 200.0f;
        info.position.y = windowHeight - 200.0f;
        //info.debug = true;
        
        const char* text =
            "New Game\n"
            "Options\n"
            "Credits\n"
            "Quit\n";

        Context::TextRenderer().Draw(info, projection, text);
    }
}