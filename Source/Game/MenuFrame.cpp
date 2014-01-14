#include "Precompiled.hpp"
#include "MenuFrame.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Context.hpp"

namespace
{
    // Menu element text.
    const char* MenuElementText[MenuFrame::MenuElements::Count] =
    {
        "Continue",
        "New Game",
        "Options",
        "Credits",
        "Quit",
    };
}

MenuFrame::MenuFrame() :
    m_elementSelected(MenuElements::None),
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

    if(!m_fontElement.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 48, 512, 512))
        return false;

    // Get window size.
    int windowWidth = Console::windowWidth.GetInteger();
    int windowHeight = Console::windowHeight.GetInteger();

    // Fill element data array.
    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Set enabled state.
        if(i == MenuElements::Quit)
        {
            element.enabled = true;
        }
        else
        {
            element.enabled = false;
        }

        // Set element text.
        element.text = MenuElementText[i];

        // Set element draw data.
        element.position.x = 200.0f;
        element.position.y = windowHeight - 185.0f - i * m_fontElement.GetLineSpacing();

        // Calculate a bounding box.
        TextRenderer::DrawInfo info;
        info.font = &m_fontElement;
        info.position = element.position;

        TextRenderer::DrawMetrics metrics = Context::TextRenderer().Measure(info, element.text);

        element.boundingBox = metrics.drawingArea;
    }

    // Success!
    m_initialized = true;

    return true;
}

void MenuFrame::Cleanup()
{
    m_elementSelected = MenuElements::None;

    m_fontTitle.Cleanup();
    m_fontElement.Cleanup();

    m_initialized = false;
}

bool MenuFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            if(m_elementSelected == MenuElements::Quit)
            {
                Context::Quit();
            }
        }
        return true;
    }

    return false;
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

    // Check which element is currently selected.
    m_elementSelected = MenuElements::None;

    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Check if element is enabled.
        if(!element.enabled)
            continue;

        // Check if the cursor is over this element.
        const glm::vec4& boundingBox = element.boundingBox;

        if(boundingBox.x <= cursorPosition.x && cursorPosition.x < boundingBox.z &&
            boundingBox.y <= cursorPosition.y && cursorPosition.y < boundingBox.w)
        {
            m_elementSelected = i;
            break;
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

    // Draw menu elements.
    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Draw an element.
        TextRenderer::DrawInfo info;
        info.font = &m_fontElement;
        
        if(m_elementSelected == i)
        {
            info.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            if(element.enabled)
            {
                info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            else
            {
                info.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            }
        }

        info.position = element.position;

        Context::TextRenderer().Draw(info, projection, element.text);
    }
}
