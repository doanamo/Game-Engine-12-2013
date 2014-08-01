#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainGlobal.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Game/GameGlobal.hpp"
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
    if(!GameContext::GetMenuFrame().Initialize())
        return false;

    // Set the frame state.
    GameContext::GetFrameState().ChangeState(&GameContext::GetMenuFrame());

    return true;
}

void MainFrame::Cleanup()
{
    // Cleanup the game context.
    GameContext::Cleanup();
}

bool MainFrame::Process(const SDL_Event& event)
{
    if(GameContext::GetFrameState().IsValid())
    {
        if(GameContext::GetFrameState().GetState()->Process(event))
            return true;
    }

    return false;
}

void MainFrame::Update(float timeDelta)
{
    if(GameContext::GetFrameState().IsValid())
    {
        GameContext::GetFrameState().GetState()->Update(timeDelta);
    }
    else
    {
        Main::Quit();
    }
}

void MainFrame::Draw()
{
    if(GameContext::GetFrameState().IsValid())
    {
        GameContext::GetFrameState().GetState()->Draw();
    }
}
