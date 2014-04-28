#include "Precompiled.hpp"
#include "LoseFrame.hpp"

#include "MainContext.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Game/GameContext.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/GameFrame.hpp"

namespace
{
    // Frame space size.
    const float FrameWidth = 1024;
    const float FrameHeight = 576;

    // Frame transistion cooldown.
    const float FrameTransitionCooldown = 1.0f;
}

LoseFrame::LoseFrame() :
    m_initialized(false),
    m_cooldownTimer(0.0f)
{
}

LoseFrame::~LoseFrame()
{
    Cleanup();
}

bool LoseFrame::Initialize()
{
    Cleanup();

    m_initialized = true;

    return true;
}

void LoseFrame::Cleanup()
{
    m_initialized = false;

    m_screenSpace.Cleanup();

    m_cooldownTimer = 0.0f;
}

bool LoseFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        {
            // Transist back to the main menu.
            if(m_cooldownTimer > FrameTransitionCooldown)
            {
                // Change to the menu frame.
                GameContext::LoseFrame().Cleanup();
                GameContext::FrameState().ChangeState(&GameContext::MenuFrame());
            }
        }
        return true;
    }

    return false;
}

void LoseFrame::Update(float timeDelta)
{
    //
    // Setup View
    //

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);
    m_screenSpace.SetTargetSize(FrameWidth, FrameHeight);

    //
    // Frame
    //

    // Update the cooldown timer.
    m_cooldownTimer += timeDelta;
}

void LoseFrame::Draw()
{
    // Clear the depth.
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Depth);

    // Clear the screen.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().Clear(ClearFlags::Color);

    // Draw the game over text.
    {
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = 128;
        info.align = TextDrawAlign::Centered;
        info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        info.outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.outlineRange = glm::vec2(0.45f, 0.55f);
        info.position.x = FrameWidth * 0.5f;
        info.position.y = FrameHeight * 0.7f;

        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), "Game Over");
    }

    {
        TextDrawInfo info;
        info.font = &Main::DefaultFont();
        info.size = 48;
        info.align = TextDrawAlign::Centered;
        info.bodyColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        info.position.x = FrameWidth * 0.5f;
        info.position.y = FrameHeight * 0.5f;

        Main::TextRenderer().Draw(info, m_screenSpace.GetTransform(), "Press any key to continue...");
    }
}
