#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Graphics/ShapeRenderer.hpp"

#include "System/FrameCounter.hpp"

#include "MainContext.hpp"

//
// Console Variables
//

namespace Console
{
    ConsoleVariable drawFrameRate("r_drawfps", true, "Displays current frame rate on the screen.");
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
    Console::windowName = "Window";
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
    // Font
    //

    // Load font file.
    Font font;
    if(!font.Load(Main::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 22, 512, 512))
        return -1;
    
    // Cache ASCII character set.
    font.CacheASCII();
    
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
        }

        // Get current window size.
        int windowWidth = Console::windowWidth.GetInteger();
        int windowHeight = Console::windowHeight.GetInteger();

        // Update frame counter.
        Main::FrameCounter().Update(dt);

        // Update cursor blink time.
        Main::TextRenderer().UpdateCursorBlink(dt);

        // Setup the viewport.
        glViewport(0, 0, windowWidth, windowHeight);

        // Calculate projection.
        glm::mat4x4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

        // Clear the screen.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw frame rate.
        if(Console::drawFrameRate.GetBool())
        {
            std::stringstream frameCounterText;
            frameCounterText << "FPS: " << std::fixed << std::setprecision(0) << Main::FrameCounter().GetFrameRate() 
                << " (" << std::setprecision(4) << Main::FrameCounter().GetFrameTime() << "s)";

            TextRenderer::DrawInfo info;
            info.font = &font;
            info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.position.x = 10.0f;
            info.position.y = 5.0f + font.GetLineSpacing();

            Main::TextRenderer().Draw(info, projection, frameCounterText.str().c_str());
        }

        // Draw console frame.
        Main::ConsoleFrame().Draw(projection);
        
        // Present the window content.
        bool verticalSync = false;

        if(Console::renderVsync.GetBool())
        {
            verticalSync = true;
        }

        SDL_GL_SetSwapInterval((int)verticalSync);
        SDL_GL_SwapWindow(Main::SystemWindow());
    }

    return 0;
}
