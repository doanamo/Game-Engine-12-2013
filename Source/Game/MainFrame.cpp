#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainContext.hpp"
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
}

void MainFrame::Draw()
{
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    glm::mat4 transform = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight);

    // Clear the back buffer.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);
}
