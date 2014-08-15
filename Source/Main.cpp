#include "Precompiled.hpp"

#include "MainGlobal.hpp"
#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"
#include "System/BaseFrame.hpp"
#include "System/FrameCounter.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Scripting/LuaEngine.hpp"

//
// Console Variables
//

namespace Console
{
    ConsoleVariable drawFrameRate("r_drawfps", true, "Displays current frame rate on the screen.");
    ConsoleVariable lockAspectRatio("r_lockaspectratio", true, "Locks aspect ratio of the display resolution.");
    ConsoleVariable horizontalAspectRatio("r_horizontalaspectratio", 16, "Horizontal aspect ratio.");
    ConsoleVariable verticalAspectRatio("r_verticalaspectratio", 9, "Vertical aspect ratio.");
    ConsoleVariable debugScreenBorders("debug_screenborders", false, "Enables debug draw of screen borders.");
}

//
// Helper Functions
//

void HandleWindowSizeChange(int oldWidth, int oldHeight, int newWidth, int newHeight)
{
    // Check if window size actually differs.
    if(oldWidth == newWidth && oldHeight == newHeight)
        return;

    // Enforce minimum size.
    newWidth = std::max(1024, newWidth);
    newHeight = std::max(576, newHeight);

    // Get aspect ratio settings.
    const int horizontalAspectRatio = Console::horizontalAspectRatio;
    const int verticalAspectRatio = Console::verticalAspectRatio;
                        
    // Change resolution to match the aspect ratio.
    if(Console::lockAspectRatio)
    {
        // Set new window size with locked aspect ratio.
        if(newWidth != oldWidth)
        {
            // Find next multiple of horizontal aspect ratio.
            while(newWidth % horizontalAspectRatio)
            {
                newWidth += 1;
            }

            // Calculate the other dimmension.
            newHeight = (int)(newWidth * ((float)verticalAspectRatio / horizontalAspectRatio) + 0.5f);
        }
        else
        if(newHeight != oldHeight)
        {
            // Find next multiple of vertical aspect ratio.
            while(newHeight % verticalAspectRatio)
            {
                newHeight += 1;
            }

            // Calculate the other dimmension.
            newWidth = (int)(newHeight * ((float)horizontalAspectRatio / verticalAspectRatio) + 0.5f);
        }
    }

    // Change window size.
    // This event is only triggered on user system resize.
    // Function below triggers another, regular size change event.
    SDL_SetWindowSize(Main::GetSystemWindow(), newWidth, newHeight);

    // Get the current window size.
    // The requested window size can't always be set.
    int windowWidth, windowHeight;
    SDL_GetWindowSize(Main::GetSystemWindow(), &windowWidth, &windowHeight);

    // Update the console variables.
    Console::windowWidth = windowWidth;
    Console::windowHeight = windowHeight;

    // Print a log message if window size actually changed.
    if(windowWidth != oldWidth || windowHeight != oldHeight)
    {
        Log() << "Resolution changed to " << windowWidth << "x" << windowHeight << ".";
    }
}

//
// Main
//

int main(int argc, char* argv[])
{
    // Report memory leaks at program exit.
    #if defined(_WINDOWS) && defined(_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    // Finalize static instances.
    ConsoleDefinition::FinalizeStatic();

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
        // Reset console system intermediate state.
        Main::GetConsoleSystem().ResetIntermediateState();

        // Run garbage collection.
        Main::GetLuaEngine().CollectGarbage(0.002f);

        //
        // Timer
        //

        // Update frame time.
        timePrevious = timeCurrent;
        timeCurrent = SDL_GetTicks();

        // Calculate elapsed time.
        uint32_t timeElapsed = timeCurrent - timePrevious;
        float dt = (float)timeElapsed / 1000.0f;

        // Delta can't be negative.
        dt = std::max(0.0f, dt);

        // Limit maximum frame step.
        dt = std::min(dt, 1.0f / 20.0f);

        //
        // Events
        //

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
                    {
                        int windowWidth = event.window.data1;
                        int windowHeight = event.window.data2;

                        HandleWindowSizeChange(Console::windowWidth, Console::windowHeight, windowWidth, windowHeight);
                    }
                    break;
                }
                break;
            }

            // Process an event by console frame.
            if(Main::GetConsoleFrame().Process(event))
                continue;

            // Call the scripting routine.
            Lua::LuaRef result = Main::GetLuaEngine().Call("Main.Process", event);
            if(result.isBoolean() && result.cast<bool>())
                continue;
        }

        // Handle console variable change for window size.
        if(Console::windowWidth.HasChanged() || Console::windowHeight.HasChanged())
        {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(Main::GetSystemWindow(), &windowWidth, &windowHeight);

            HandleWindowSizeChange(windowWidth, windowHeight, Console::windowWidth, Console::windowHeight);
        }

        //
        // Update
        //

        // Update frame counter.
        Main::GetFrameCounter().Update(dt);

        // Update cursor blink time.
        Main::GetTextRenderer().UpdateCursorBlink(dt);

        // Call the scripting routine.
        Main::GetLuaEngine().Call("Main.Update", dt);

        //
        // Setup View
        //

        // Setup screen space.
        Main::GetScreenSpace().SetSourceSize(Console::windowWidth, Console::windowHeight);
        Main::GetScreenSpace().SetTargetAspect((float)Console::horizontalAspectRatio / (float)Console::verticalAspectRatio);

        // Setup the viewport.
        glm::ivec4 viewport(0, 0, Console::windowWidth, Console::windowHeight);
        glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

        // Clear the screen.
        Main::GetCoreRenderer().SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        Main::GetCoreRenderer().Clear(ClearFlags::Color);

        // Get screen space transform matrices.
        glm::mat4x4 projection = Main::GetScreenSpace().GetProjection();
        glm::mat4x4 view = Main::GetScreenSpace().GetView();
        glm::mat4x4 transform = Main::GetScreenSpace().GetTransform();

        // Enable scissor area where drawing will be limited.
        glEnable(GL_SCISSOR_TEST);

        glm::vec3 position = glm::project(glm::vec3(0.0f, 0.0f, 0.0f), view, projection, viewport);
        glm::vec3 size = glm::project(glm::vec3(Main::GetScreenSpace().GetTargetSize(), 0.0f), view, projection, viewport) - position;

        glScissor((int)(position.x + 0.5f), (int)(position.y + 0.5f), (int)(size.x + 0.5f), (int)(size.y + 0.5f));
    
        SCOPE_GUARD(glDisable(GL_SCISSOR_TEST));

        //
        // Draw
        //

        // Call the scripting routine.
        Main::GetLuaEngine().Call("Main.Draw");

        // Draw frame rate.
        if(Console::drawFrameRate)
        {
            std::stringstream frameCounterText;
            frameCounterText << "FPS: " << std::fixed << std::setprecision(0) << Main::GetFrameCounter().GetFrameRate() 
                << " (" << std::setprecision(4) << Main::GetFrameCounter().GetFrameTime() << "s)";

            TextDrawInfo info;
            info.font = &Main::GetDefaultFont();
            info.size = 22;
            info.bodyColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            info.glowColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.glowRange = glm::vec2(0.2f, 0.5f);
            info.position.x = 10.0f;
            info.position.y = 5.0f + Main::GetDefaultFont().GetLineSpacing() * Main::GetDefaultFont().GetScaling(info.size);

            Main::GetTextRenderer().Draw(info, frameCounterText.str().c_str(), transform);
        }

        // Draw debug screen target borders.
        if(Console::debugScreenBorders)
        {
            BasicRenderer::RectangleStyle style;
            style.drawMode = BasicRenderer::DrawMode::Line;
            style.lineType = BasicRenderer::LineType::Adjusted;
            style.alphaBlend = false;

            BasicRenderer::Rectangle rectangle;
            rectangle.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            rectangle.bottomleft = glm::vec2(0.0f, 0.0f);
            rectangle.topright = Main::GetScreenSpace().GetTargetSize() - glm::vec2(1.0f, 1.0f);

            Main::GetBasicRenderer().DrawRectangles(style, &rectangle, 1, transform);
        }

        // Draw console frame on top of everything else.
        Main::GetConsoleFrame().Draw(transform, Main::GetScreenSpace().GetTargetSize());

        // Present the window content.
        bool verticalSync = Console::renderVsync;
        SDL_GL_SetSwapInterval((int)verticalSync);
        SDL_GL_SwapWindow(Main::GetSystemWindow());
    }

    // Everything will get cleaned up automatically.
    return 0;
}
