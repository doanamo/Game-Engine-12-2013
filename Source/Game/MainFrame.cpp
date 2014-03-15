#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainContext.hpp"
#include "GameContext.hpp"

#include "MenuFrame.hpp"

#include "Graphics/CoreRenderer.hpp"

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

    // Initialize the menu frame.
    if(!Game::MenuFrame().Initialize())
        return false;

    // Set the frame state.
    Game::FrameState().ChangeState(&Game::MenuFrame());

    return true;
}

void MainFrame::Cleanup()
{
    Game::MenuFrame().Cleanup();
}

bool MainFrame::Process(const SDL_Event& event)
{
    if(Game::FrameState().IsValid())
    {
        if(Game::FrameState().GetState()->Process(event))
            return true;
    }

    return false;
}

void MainFrame::Update(float dt)
{
    if(Game::FrameState().IsValid())
    {
        Game::FrameState().GetState()->Update(dt);
    }
    else
    {
        Main::Quit();
    }
}

void MainFrame::Draw()
{
    if(Game::FrameState().IsValid())
    {
        Game::FrameState().GetState()->Draw();
    }
}
