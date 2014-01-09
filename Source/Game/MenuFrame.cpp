#include "Precompiled.hpp"
#include "MenuFrame.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Context.hpp"

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

    if(!m_fontOption.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 48, 512, 512))
        return false;

    // Calculate menu option data.
    m_optionText[0] = "New Game";
    m_optionText[1] = "Options";
    m_optionText[2] = "Credits";
    m_optionText[3] = "Quit";

    for(int i = 0; i < OptionCount; ++i)
    {
        // Get window size.
        int windowWidth = Console::windowWidth.GetInteger();
        int windowHeight = Console::windowHeight.GetInteger();

        // Calculate draw info.
        TextRenderer::DrawInfo& info = m_optionDrawInfo[i];
        info.font = &m_fontOption;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 200.0f;
        info.position.y = windowHeight - 200.0f - i * m_fontOption.GetLineSpacing();
        //info.debug = true;
    }

    for(int i = 0; i < OptionCount; ++i)
    {
        TextRenderer::DrawMetrics metrics = Context::TextRenderer().Measure(m_optionDrawInfo[i], m_optionText[i]);

        m_optionBoundingBox[i] = metrics.drawingArea;
    }

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
    // Get cursor position.
    glm::ivec2 cursorPosition;
    SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);

    // Flip cursor y-axis.
    int windowWidth, windowHeight;
    SDL_GetWindowSize(Context::SystemWindow(), &windowWidth, &windowHeight);

    cursorPosition.y = (windowHeight - 1) - cursorPosition.y;

    // Check 
    static const glm::vec4 DefaultColor(0.0f, 0.0f, 0.0f, 1.0f);
    static const glm::vec4 SelectedColor(1.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i < OptionCount; ++i)
    {
        const glm::vec4& boundingBox = m_optionBoundingBox[i];

        if(boundingBox.x <= cursorPosition.x && cursorPosition.x < boundingBox.z &&
            boundingBox.y <= cursorPosition.y && cursorPosition.y < boundingBox.w)
        {
            m_optionDrawInfo[i].color = SelectedColor;
        }
        else
        {
            m_optionDrawInfo[i].color = DefaultColor;
        }
    }
}

void MenuFrame::Draw(const glm::mat4& projection)
{
    // Get window size.
    int windowWidth = Console::windowWidth.GetInteger();
    int windowHeight = Console::windowHeight.GetInteger();

    // Draw menu title.
    {
        TextRenderer::DrawInfo info;
        info.font = &m_fontTitle;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 50.0f;
        info.position.y = windowHeight - 10.0f;

        Context::TextRenderer().Draw(info, projection, "Awesome Game");
    }

    // Draw menu options.
    for(int i = 0; i < OptionCount; ++i)
    {
        Context::TextRenderer().Draw(m_optionDrawInfo[i], projection, m_optionText[i]);
    }
}
