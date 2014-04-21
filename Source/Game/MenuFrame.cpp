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

    // Game space size.
    float gameWidth = 1024;
    float gameHeight = 576;
}

MenuFrame::MenuFrame() :
    m_projection(1.0f),
    m_view(1.0f),
    m_transform(1.0f),
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
        element.position.y = gameHeight - 195.0f - i * Main::DefaultFont().GetLineSpacing() * Main::DefaultFont().GetScaling(OptionFontSize);

        // Calculate a bounding box.
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = OptionFontSize;
        info.position = element.position;

        TextDrawMetrics metrics = Main::TextRenderer().Measure(info, element.text);

        element.boundingBox = metrics.textArea;
    }

    // Success!
    m_initialized = true;

    return true;
}

void MenuFrame::Cleanup()
{
    m_screenSpace.Cleanup();

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
    //
    // Setup View
    //

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);
    m_screenSpace.SetTargetSize(1024.0f, 576.0f);

    // Setup matrices.
    glm::vec4 screenSpace = m_screenSpace.GetRectangle();
    m_projection = glm::ortho(screenSpace.x, screenSpace.y, screenSpace.z, screenSpace.w);
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(m_screenSpace.GetOffset(), 0.0f));
    m_transform = m_projection * m_view;

    //
    // Menu Elements
    //

    // Get cursor position.
    glm::ivec2 windowCursorPosition;
    SDL_GetMouseState(&windowCursorPosition.x, &windowCursorPosition.y);

    // Flip cursor y-axis.
    windowCursorPosition.y = ((int)windowHeight - 1) - windowCursorPosition.y;

    // Unproject cursor position to game space.
    glm::vec4 viewport(0.0f, 0.0f, Console::windowWidth, Console::windowHeight);
    glm::vec3 cursorPosition = glm::unProject(glm::vec3(windowCursorPosition.x, windowCursorPosition.y, 0.0f), m_view, m_projection, viewport);

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
    // Clear the depth.
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Depth);

    // Clear the screen.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().Clear(ClearFlags::Color);

    // Draw menu title.
    {
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = TitleFontSize;
        info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        info.outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.outlineRange = glm::vec2(0.45f, 0.55f);
        info.position.x = 50.0f;
        info.position.y = gameHeight + 10.0f;

        Main::TextRenderer().Draw(info, m_transform, "Gunstar");
    }

    // Draw menu elements.
    for(int i = 0; i < MenuElements::Count; ++i)
    {
        ElementData& element = m_elements[i];

        // Draw an element.
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = OptionFontSize;

        info.outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.outlineRange = glm::vec2(0.4f, 0.5f);
        
        if(m_elementSelected == i)
        {
            info.bodyColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            if(element.enabled)
            {
                info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                info.bodyColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.1f);
            }
        }

        info.position = element.position;

        Main::TextRenderer().Draw(info, m_transform, element.text);
    }
}
