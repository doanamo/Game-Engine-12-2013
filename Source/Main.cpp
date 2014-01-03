#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Font.hpp"
#include "ShapeRenderer.hpp"
#include "TextRenderer.hpp"

#include "FrameCounter.hpp"

//
// Console Definitions
//

ConsoleVariable mouseSensivity("i_sensivity", "4.2", "Mouse sensivity.");
ConsoleVariable playerHealth("g_playerhealth", "100", "Current player health.");
ConsoleVariable playerAlive("g_playeralive", "true", "Is player alive?");
ConsoleVariable userProfile("s_userprofile", "./", "Current user's profile directory.");

//
// Main
//

int main(int argc, char* argv[])
{
    //
    // Context
    //
    
    if(!Context::Initialize())
        return -1;

    SCOPE_GUARD(Context::Cleanup());

    //
    // Frame Counter
    //

    // Initialize the frame counter.
    FrameCounter frameCounter;
    if(!frameCounter.Initialize(30))
        return -1;

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
    // Test
    //

    // Projection.
    glm::mat4x4 projection = glm::ortho(0.0f, (float)1024, 0.0f, (float)576);

    //
    // Example
    //

    if(argc >= 1)
    {
        userProfile.SetString(argv[0]);
    }

    Log() << mouseSensivity.GetName() << " = " << mouseSensivity.GetString();
    Log() << playerHealth.GetName() << " = " << playerHealth.GetString();
    Log() << playerAlive.GetName() << " = " << playerAlive.GetString();
    
    //
    // Main Loop
    //

    uint32_t timePrevious = SDL_GetTicks();
    uint32_t timeCurrent = timePrevious;

    while(!Context::IsQuitting())
    {
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

        // Calculate elapsed time.
        uint32_t timeElapsed = timeCurrent - timePrevious;
        float dt = (float)timeElapsed / 1000.0f;

        // Update frame counter.
        frameCounter.Update(dt);

        // Update cursor blink time.
        Context::TextRenderer().Update(dt);

        // Setup the viewport.
        int windowWidth, windowHeight;
        SDL_GetWindowSize(Context::SystemWindow(), &windowWidth, &windowHeight);

        glViewport(0, 0, windowWidth, windowHeight);

        // Clear the screen.
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw text.
        {
            const char* text0 = 
                "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. "
                "Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, "
                "ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, "
                "fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, "
                "justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper "
                "nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. "
                "Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius "
                "laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies "
                "nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, "
                "sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. "
                "Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. "
                "Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales "
                "sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc.";

            TextRenderer::DrawInfo info;
            info.font = &font;
            info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.position.x = 10.0f;
            info.position.y = 576.0f - 10.0f;
            info.size.x = 800.0f;
            info.size.y = 0.0f;
            info.debug = true;

            Context::TextRenderer().Draw(info, projection, text0);
        }

        // Draw console frame.
        Context::ConsoleFrame().Draw(projection);

        // Draw frame rate.
        {
            std::stringstream frameCounterText;
            frameCounterText << "FPS: " << std::fixed << std::setprecision(0) << frameCounter.GetFrameRate() 
                << " (" << std::setprecision(4) << frameCounter.GetFrameTime() << "s)";

            TextRenderer::DrawInfo info;
            info.font = &font;
            info.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            info.position.x = 10.0f;
            info.position.y = 5.0f + font.GetLineSpacing();

            Context::TextRenderer().Draw(info, projection, frameCounterText.str().c_str());
        }
        
        // Present the window content.
        SDL_GL_SetSwapInterval(0);
        SDL_GL_SwapWindow(Context::SystemWindow());

        // Update frame time.
        timePrevious = timeCurrent;
        timeCurrent = SDL_GetTicks();
    }

    return 0;
}
