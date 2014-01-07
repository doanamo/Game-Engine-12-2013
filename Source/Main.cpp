#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Graphics/ShapeRenderer.hpp"

#include "System/FrameCounter.hpp"

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
    Console::windowResize = false;

    //
    // Context
    //
    
    // Initialize the context.
    if(!Context::Initialize())
        return -1;

    SCOPE_GUARD(Context::Cleanup());

    //
    // Font
    //

    // Load font file.
    Font font;
    if(!font.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 22, 512, 512))
        return -1;
    
    // Cache ASCII character set.
    font.CacheASCII();
    
    //
    // Main Loop
    //

    uint32_t timePrevious = SDL_GetTicks();
    uint32_t timeCurrent = timePrevious;

    while(!Context::IsQuitting())
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
                Context::Quit();
                break;
            }

            // Process an event by console frame.
            Context::ConsoleFrame().Process(event);
        }

        // Get current window size.
        int windowWidth, windowHeight;
        SDL_GetWindowSize(Context::SystemWindow(), &windowWidth, &windowHeight);

        // Set console variables.
        Console::windowWidth.SetInteger(windowWidth);
        Console::windowHeight.SetInteger(windowHeight);

        // Update frame counter.
        Context::FrameCounter().Update(dt);

        // Update cursor blink time.
        Context::TextRenderer().Update(dt);

        // Setup the viewport.
        glViewport(0, 0, windowWidth, windowHeight);

        // Calculate projection.
        glm::mat4x4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

        // Clear the screen.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw frame rate.
        {
            std::stringstream frameCounterText;
            frameCounterText << "FPS: " << std::fixed << std::setprecision(0) << Context::FrameCounter().GetFrameRate() 
                << " (" << std::setprecision(4) << Context::FrameCounter().GetFrameTime() << "s)";

            TextRenderer::DrawInfo info;
            info.font = &font;
            info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.position.x = 10.0f;
            info.position.y = 5.0f + font.GetLineSpacing();

            Context::TextRenderer().Draw(info, projection, frameCounterText.str().c_str());
        }

        // Draw console frame.
        Context::ConsoleFrame().Draw(projection);
        
        // Present the window content.
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SwapWindow(Context::SystemWindow());
    }

    return 0;
}
