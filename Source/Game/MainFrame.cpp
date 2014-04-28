#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainContext.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Game/GameContext.hpp"
#include "Game/MenuFrame.hpp"

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

    // Initialize the game context.
    if(!GameContext::Initialize())
        return false;

    // Initialize the menu frame.
    if(!GameContext::MenuFrame().Initialize())
        return false;

    // Set the frame state.
    GameContext::FrameState().ChangeState(&GameContext::MenuFrame());

    return true;
}

void MainFrame::Cleanup()
{
    // Cleanup the game context.
    GameContext::Cleanup();
}

bool MainFrame::Process(const SDL_Event& event)
{
    if(GameContext::FrameState().IsValid())
    {
        if(GameContext::FrameState().GetState()->Process(event))
            return true;
    }

    return false;
}

void MainFrame::Update(float timeDelta)
{
    if(GameContext::FrameState().IsValid())
    {
        GameContext::FrameState().GetState()->Update(timeDelta);
    }
    else
    {
        Main::Quit();
    }
}

void MainFrame::Draw()
{
    if(GameContext::FrameState().IsValid())
    {
        GameContext::FrameState().GetState()->Draw();
    }
}
