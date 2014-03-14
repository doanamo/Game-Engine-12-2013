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

    Main::FrameState().ChangeState(&Game::MenuFrame());

    return true;
}

void MainFrame::Cleanup()
{
}

bool MainFrame::Process(const SDL_Event& event)
{
    return false;
}

void MainFrame::Update(float dt)
{
    Main::Quit();
}

void MainFrame::Draw()
{
}
