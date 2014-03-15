#include "Precompiled.hpp"
#include "MenuFrame.hpp"
#include "GameFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

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

    // Font sizes.
    const float TitleFontSize = 176;
    const float OptionFontSize = 48;
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
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!m_initialized)
        {
            Cleanup();
        }
    });

    // Get window size.
    int windowWidth = Console::windowWidth;
    int windowHeight = Console::windowHeight;

    // Fill element data array.
    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Set enabled state.
        if(i == MenuElements::NewGame || i == MenuElements::Quit)
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
        element.position.x = 175.0f;
        element.position.y = windowHeight - 195.0f - i * Main::DefaultFont().GetLineSpacing() * Main::DefaultFont().GetScaling(OptionFontSize);

        // Calculate a bounding box.
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = OptionFontSize;
        info.position = element.position;

        TextDrawMetrics metrics = Main::TextRenderer().Measure(info, element.text);

        element.boundingBox = metrics.drawingArea;
    }

    // Success!
    m_initialized = true;

    return true;
}

void MenuFrame::Cleanup()
{
    m_elementSelected = MenuElements::None;

    m_initialized = false;
}

void MenuFrame::OnEnter()
{
    // Enable "Continue" element if the game is active.
    m_elements[MenuElements::Continue].enabled = Game::GameFrame().IsInitialized();
}

bool MenuFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if(event.button.button == SDL_BUTTON_LEFT)
        {
            switch(m_elementSelected)
            {
            case MenuElements::Continue:
                {
                    assert(Game::GameFrame().IsInitialized());

                    // Switch to the game frame.
                    Game::FrameState().ChangeState(&Game::GameFrame());
                }
                break;

            case MenuElements::NewGame:
                {
                    // Initialize the game frame.
                    Game::GameFrame().Initialize();
                    
                    // Switch to the game frame.
                    Game::FrameState().ChangeState(&Game::GameFrame());
                }
                break;

            case MenuElements::Quit:
                Main::Quit();
                break;
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
    SDL_GetWindowSize(Main::SystemWindow(), &windowWidth, &windowHeight);

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

void MenuFrame::Draw()
{
    // Get window size.
    int windowWidth = Console::windowWidth;
    int windowHeight = Console::windowHeight;

    // Calculate projection.
    glm::mat4x4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

    // Clear the screen.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);

    // Draw menu title.
    {
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = TitleFontSize;
        info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = 50.0f;
        info.position.y = windowHeight + 10.0f;

        Main::TextRenderer().Draw(info, projection, "Gunstar");
    }

    // Draw menu elements.
    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Draw an element.
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = OptionFontSize;
        
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

        Main::TextRenderer().Draw(info, projection, element.text);
    }
}
