#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Font.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Graphics/ShapeRenderer.hpp"

#include "System/FrameCounter.hpp"

#include "Common/StateMachine.hpp"
#include "Game/BaseFrame.hpp"
#include "Game/Game.hpp"

#include "Context.hpp"

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
    Console::windowName = "Game";
    Console::windowWidth = 1024;
    Console::windowHeight = 576;
    Console::windowResize = false;

    // Render settings.
    Console::renderVsync = true;

    //
    // Context
    //
    
    // Initialize the context.
    if(!Context::Initialize())
        return -1;

    SCOPE_GUARD(Context::Cleanup());

    //
    // Game
    //

    // Initialize the game.
    if(!Game::Initialize())
        return -1;

    SCOPE_GUARD(Game::Cleanup());

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

            case SDL_WINDOWEVENT_RESIZED:
                Console::windowWidth.SetInteger(event.window.data1);
                Console::windowHeight.SetInteger(event.window.data2);
                break;
            }

            // Process an event by console frame.
            if(Context::ConsoleFrame().Process(event))
                continue;

            // Process an event by the current state frame.
            if(Game::StateMachine().GetState()->Process(event))
                continue;
        }

        // Get current window size.
        int windowWidth = Console::windowWidth.GetInteger();
        int windowHeight = Console::windowHeight.GetInteger();

        // Update frame counter.
        Context::FrameCounter().Update(dt);

        // Update cursor blink time.
        Context::TextRenderer().UpdateCursorBlink(dt);

        // Update the current state frame.
        Game::StateMachine().GetState()->Update(dt);

        // Setup the viewport.
        glViewport(0, 0, windowWidth, windowHeight);

        // Calculate projection.
        glm::mat4x4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

        // Clear the screen.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the current state frame.
        Game::StateMachine().GetState()->Draw(projection);

        // Draw frame rate.
        if(Console::drawFrameRate.GetBool())
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
        bool verticalSync = false;

        if(Console::renderVsync.GetBool())
        {
            verticalSync = true;
        }

        SDL_GL_SetSwapInterval((int)verticalSync);
        SDL_GL_SwapWindow(Context::SystemWindow());
    }

    return 0;
}
