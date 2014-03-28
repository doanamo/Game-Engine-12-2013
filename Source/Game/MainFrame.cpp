#include "Precompiled.hpp"
#include "MainFrame.hpp"

#include "MainContext.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

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
    //
    // Setup View
    //

    // Get window size.
    float windowWidth = Console::windowWidth;
    float windowHeight = Console::windowHeight;

    // Set target size.
    float targetWidth = 1410.0f;
    float targetHeight = 900.0f;

    // Setup screen space.
    ScreenSpace screenSpace;
    screenSpace.SetSourceSize(windowWidth, windowHeight);
    screenSpace.SetTargetSize(targetWidth, targetHeight);

    // Setup matrices.
    glm::vec4 rectangle = screenSpace.GetRectangle();
    glm::mat4 projection = glm::ortho(rectangle.x, rectangle.y, rectangle.z, rectangle.w);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(screenSpace.GetOffset(), 0.0f));
    glm::mat4 transform = projection * view;

    //
    // Draw Text
    //

    // Clear the back buffer.
    Main::CoreRenderer().SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Main::CoreRenderer().SetClearDepth(1.0f);
    Main::CoreRenderer().Clear(ClearFlags::Color | ClearFlags::Depth);

    // Draw some text.
    TextDrawInfo drawInfo;
    drawInfo.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    drawInfo.font = &Main::DefaultFont();
    drawInfo.size = 128;
    drawInfo.position = glm::vec2(targetWidth * 0.5f, targetHeight * 0.5f);
    drawInfo.debug = true;

    Main::TextRenderer().Draw(drawInfo, transform, "Centered");
}
