#include "Precompiled.hpp"
#include "MenuFrame.hpp"

#include "MainContext.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Game/GameContext.hpp"
#include "Game/GameFrame.hpp"

namespace
{
    // Font sizes.
    const float TitleFontSize = 176;
    const float ButtonFontSize = 48;

    // View space size.
    float viewWidth = 1024;
    float viewHeight = 576;
}

MenuFrame::MenuFrame() :
    m_initialized(false)
{
}

MenuFrame::~MenuFrame()
{
    Cleanup();
}

void MenuFrame::Cleanup()
{
    m_screenSpace.Cleanup();

    m_interfaceRoot.Cleanup();

    m_buttonContinue.Cleanup();
    m_buttonNewGame.Cleanup();
    m_buttonOptions.Cleanup();
    m_buttonCredits.Cleanup();
    m_buttonQuit.Cleanup();

    m_actionContinue.Cleanup();
    m_actionNewGame.Cleanup();
    m_actionQuit.Cleanup();

    m_initialized = false;
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

    // Set interface screen space.
    m_interfaceRoot.SetScreenSpace(&m_screenSpace);

    // Setup interface elements.
    glm::vec2 currentPosition(175.0f, viewHeight - 260.0f);

    {
        const char* buttonText = "Continue";

        TextDrawInfo textDrawInfo;
        textDrawInfo.font = &Main::DefaultFont();
        textDrawInfo.size = ButtonFontSize;
        textDrawInfo.position = currentPosition;
        textDrawInfo.align = TextDrawAlign::BottomLeft;

        TextDrawMetrics textDrawMetrics;
        textDrawMetrics = Main::TextRenderer().Measure(textDrawInfo, buttonText);

        glm::vec2 size;
        size.x = textDrawMetrics.textArea.z - textDrawMetrics.textArea.x;
        size.y = textDrawMetrics.textArea.w - textDrawMetrics.textArea.y;

        m_buttonContinue.SetEnabled(false);
        m_buttonContinue.SetText(buttonText);
        m_buttonContinue.SetPosition(currentPosition);
        m_buttonContinue.SetSize(size);

        currentPosition.y -= size.y;
    }

    {
        const char* buttonText = "New Game";

        TextDrawInfo textDrawInfo;
        textDrawInfo.font = &Main::DefaultFont();
        textDrawInfo.size = ButtonFontSize;
        textDrawInfo.position = currentPosition;
        textDrawInfo.align = TextDrawAlign::BottomLeft;

        TextDrawMetrics textDrawMetrics;
        textDrawMetrics = Main::TextRenderer().Measure(textDrawInfo, buttonText);

        glm::vec2 size;
        size.x = textDrawMetrics.textArea.z - textDrawMetrics.textArea.x;
        size.y = textDrawMetrics.textArea.w - textDrawMetrics.textArea.y;

        m_buttonNewGame.SetEnabled(true);
        m_buttonNewGame.SetText(buttonText);
        m_buttonNewGame.SetPosition(currentPosition);
        m_buttonNewGame.SetSize(size);

        currentPosition.y -= size.y;
    }

    {
        const char* buttonText = "Options";

        TextDrawInfo textDrawInfo;
        textDrawInfo.font = &Main::DefaultFont();
        textDrawInfo.size = ButtonFontSize;
        textDrawInfo.position = currentPosition;
        textDrawInfo.align = TextDrawAlign::BottomLeft;

        TextDrawMetrics textDrawMetrics;
        textDrawMetrics = Main::TextRenderer().Measure(textDrawInfo, buttonText);

        glm::vec2 size;
        size.x = textDrawMetrics.textArea.z - textDrawMetrics.textArea.x;
        size.y = textDrawMetrics.textArea.w - textDrawMetrics.textArea.y;

        m_buttonOptions.SetEnabled(false);
        m_buttonOptions.SetText(buttonText);
        m_buttonOptions.SetPosition(currentPosition);
        m_buttonOptions.SetSize(size);

        currentPosition.y -= size.y;
    }

    {
        const char* buttonText = "Credits";

        TextDrawInfo textDrawInfo;
        textDrawInfo.font = &Main::DefaultFont();
        textDrawInfo.size = ButtonFontSize;
        textDrawInfo.position = currentPosition;
        textDrawInfo.align = TextDrawAlign::BottomLeft;

        TextDrawMetrics textDrawMetrics;
        textDrawMetrics = Main::TextRenderer().Measure(textDrawInfo, buttonText);

        glm::vec2 size;
        size.x = textDrawMetrics.textArea.z - textDrawMetrics.textArea.x;
        size.y = textDrawMetrics.textArea.w - textDrawMetrics.textArea.y;

        m_buttonCredits.SetEnabled(false);
        m_buttonCredits.SetText(buttonText);
        m_buttonCredits.SetPosition(currentPosition);
        m_buttonCredits.SetSize(size);

        currentPosition.y -= size.y;
    }

    {
        const char* buttonText = "Quit";

        TextDrawInfo textDrawInfo;
        textDrawInfo.font = &Main::DefaultFont();
        textDrawInfo.size = ButtonFontSize;
        textDrawInfo.position = currentPosition;
        textDrawInfo.align = TextDrawAlign::BottomLeft;

        TextDrawMetrics textDrawMetrics;
        textDrawMetrics = Main::TextRenderer().Measure(textDrawInfo, buttonText);

        glm::vec2 size;
        size.x = textDrawMetrics.textArea.z - textDrawMetrics.textArea.x;
        size.y = textDrawMetrics.textArea.w - textDrawMetrics.textArea.y;

        m_buttonQuit.SetEnabled(true);
        m_buttonQuit.SetText(buttonText);
        m_buttonQuit.SetPosition(currentPosition);
        m_buttonQuit.SetSize(size);

        currentPosition.y -= size.y;
    }

    // Add buttons to the interface root.
    m_interfaceRoot.AddChild(&m_buttonContinue);
    m_interfaceRoot.AddChild(&m_buttonNewGame);
    m_interfaceRoot.AddChild(&m_buttonOptions);
    m_interfaceRoot.AddChild(&m_buttonCredits);
    m_interfaceRoot.AddChild(&m_buttonQuit);

    // Setup event receivers.
    m_actionContinue.Bind<MenuFrame, &MenuFrame::ButtonContinue>(this);
    m_actionNewGame.Bind<MenuFrame, &MenuFrame::ButtonNewGame>(this);
    m_actionQuit.Bind<MenuFrame, &MenuFrame::ButtonQuit>(this);

    m_buttonContinue.OnEventAction(m_actionContinue);
    m_buttonNewGame.OnEventAction(m_actionNewGame);
    m_buttonQuit.OnEventAction(m_actionQuit);

    // Success!
    m_initialized = true;

    return true;
}

void MenuFrame::ButtonContinue(const Button::EventAction& event)
{
    assert(m_initialized);
    assert(GameContext::GameFrame().IsInitialized());

    // Switch back to the game frame.
    GameContext::FrameState().ChangeState(&GameContext::GameFrame());
}

void MenuFrame::ButtonNewGame(const Button::EventAction& event)
{
    assert(m_initialized);

    // Initialize the game frame.
    Log() << "Starting a new game...";

    GameContext::GameFrame().Initialize();

    // Enable the continue button.
    m_buttonContinue.Enable();
                    
    // Switch to the game frame.
    GameContext::FrameState().ChangeState(&GameContext::GameFrame());
}

void MenuFrame::ButtonQuit(const Button::EventAction& event)
{
    assert(m_initialized);

    // Quit the application.
    Main::Quit();
}

void MenuFrame::OnEnter()
{
    // Enable the continue button if the game is active.
    bool isGameRunning = GameContext::GameFrame().IsInitialized();
    m_buttonContinue.SetEnabled(isGameRunning);
}

bool MenuFrame::Process(const SDL_Event& event)
{
    if(m_interfaceRoot.Process(event))
        return true;

    return false;
}

void MenuFrame::Update(float timeDelta)
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
        info.position.y = viewHeight + 10.0f;

        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), "Gunstar");
    }

    // Define button drawing routine.
    auto DrawButton = [&](const Button& button)
    {
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        
        info.position = button.GetPosition();
        info.size = ButtonFontSize;
        info.align = TextDrawAlign::BottomLeft;

        info.outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.outlineRange = glm::vec2(0.4f, 0.5f);

        if(button.IsHovered())
        {
            info.bodyColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            if(button.IsEnabled())
            {
                info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                info.bodyColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.1f);
            }
        }

        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), button.GetText().c_str());
    };

    // Draw buttons.
    DrawButton(m_buttonContinue);
    DrawButton(m_buttonNewGame);
    DrawButton(m_buttonOptions);
    DrawButton(m_buttonCredits);
    DrawButton(m_buttonQuit);
}
