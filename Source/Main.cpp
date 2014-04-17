#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Graphics/ShapeRenderer.hpp"

#include "System/BaseFrame.hpp"
#include "System/FrameCounter.hpp"

#include "MainContext.hpp"

//
// Console Variables
//

namespace Console
{
    ConsoleVariable drawFrameRate("r_drawfps", true, "Displays current frame rate on the screen.");
    ConsoleVariable debugScreenBorders("debug_screenborders", false, "Enables debug draw of screen borders.");
}

//
// Main
//

int main(int argc, char* argv[])
{
    //
    // Configuration
    //

    // Window settings.
    Console::windowName = "Game";
    Console::windowWidth = 1024;
    Console::windowHeight = 576;
    Console::windowResize = true;

    // Render settings.
    Console::renderVsync = true;

    //
    // Context
    //
    
    // Initialize the main context.
    if(!Main::Initialize())
        return -1;

    SCOPE_GUARD(Main::Cleanup());
    
    //
    // Main Loop
    //

    uint32_t timePrevious = SDL_GetTicks();
    uint32_t timeCurrent = timePrevious;

    while(!Main::IsQuitting())
    {
        // Update frame time.
        timePrevious = timeCurrent;
        timeCurrent = SDL_GetTicks();

        // Calculate elapsed time.
        uint32_t timeElapsed = timeCurrent - timePrevious;
        float dt = (float)timeElapsed / 1000.0f;

        // Limit maximum frame step.
        dt = std::min(dt, 1.0f / 20.0f);

        // Handle window events.
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                Main::Quit();
                break;

            case SDL_WINDOWEVENT:
                switch(event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    Console::windowWidth = event.window.data1;
                    Console::windowHeight = event.window.data2;
                    break;
                }
                break;
            }

            // Process an event by console frame.
            if(Main::ConsoleFrame().Process(event))
                continue;

            // Process an event by main frame.
            if(Main::MainFrame().Process(event))
                continue;
        }

        // Get current window size.
        int windowWidth = Console::windowWidth;
        int windowHeight = Console::windowHeight;

        // Setup screen space.
        Main::ScreenSpace().SetSourceSize((float)windowWidth, (float)windowHeight);
        Main::ScreenSpace().SetTargetAspect(16.0f / 9.0f);

        // Update frame counter.
        Main::FrameCounter().Update(dt);

        // Update cursor blink time.
        Main::TextRenderer().UpdateCursorBlink(dt);

        // Update main frame.
        Main::MainFrame().Update(dt);

        // Setup the viewport.
        glm::ivec4 viewport(0, 0, windowWidth, windowHeight);
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

        // Clear the screen.
        Main::CoreRenderer().SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        Main::CoreRenderer().Clear(ClearFlags::Color);

        // Calculate projection.
        glm::mat4x4 projection = Main::ScreenSpace().GetProjection();
        glm::mat4x4 view = Main::ScreenSpace().GetView();
        glm::mat4x4 transform = Main::ScreenSpace().GetTransform();

        // Calculate scissor area.
        glm::vec3 position = glm::project(glm::vec3(0.0f, 0.0f, 0.0f), view, projection, viewport);
        glm::vec3 size = glm::project(glm::vec3(Main::ScreenSpace().GetTargetSize(), 0.0f), view, projection, viewport) - position;

        glScissor((int)(position.x + 0.5f), (int)(position.y + 0.5f), (int)(size.x + 0.5f), (int)(size.y + 0.5f));

        // Toggle scissor test.
        glEnable(GL_SCISSOR_TEST);
    
        SCOPE_GUARD(glDisable(GL_SCISSOR_TEST));

        // Draw the main frame.
        Main::MainFrame().Draw();

        // Draw frame rate.
        if(Console::drawFrameRate)
        {
            std::stringstream frameCounterText;
            frameCounterText << "FPS: " << std::fixed << std::setprecision(0) << Main::FrameCounter().GetFrameRate() 
                << " (" << std::setprecision(4) << Main::FrameCounter().GetFrameTime() << "s)";

            TextDrawInfo info;
            info.font = &Main::DefaultFont();
            info.size = 22;
            info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            //info.outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            //info.outlineRange = glm::vec2(0.3f, 0.5f);
            info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.glowRange = glm::vec2(0.2f, 0.5f);
            info.position.x = 10.0f;
            info.position.y = 5.0f + Main::DefaultFont().GetLineSpacing() * Main::DefaultFont().GetScaling(info.size);

            Main::TextRenderer().Draw(info, transform, frameCounterText.str().c_str());
        }

        // Draw debug screen target borders.
        if(Console::debugScreenBorders)
        {
            ShapeRenderer::Rectangle rectangle;
            rectangle.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            rectangle.position = glm::vec2(0.0f, 0.0f);
            rectangle.size = Main::ScreenSpace().GetTargetSize();

            Main::ShapeRenderer().DrawRectangles(&rectangle, 1, transform);
        }

        // Draw console frame.
        Main::ConsoleFrame().Draw(transform, Main::ScreenSpace().GetTargetSize());

        // Present the window content.
        bool verticalSync = false;

        if(Console::renderVsync)
        {
            verticalSync = true;
        }

        SDL_GL_SetSwapInterval((int)verticalSync);
        SDL_GL_SwapWindow(Main::SystemWindow());
    }

    return 0;
}
