#include "Precompiled.hpp"
#include "Context.hpp"

#include "Logger/Logger.hpp"
#include "Logger/LoggerOutputFile.hpp"
#include "Logger/LoggerOutputConsole.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleHistory.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Texture.hpp"

#include "Graphics/ShapeRenderer.hpp"
#include "Graphics/TextRenderer.hpp"

#include "System/FrameCounter.hpp"

//
// Console Variables
//

namespace Console
{
    ConsoleVariable windowName(ConsoleDefinition::Internal, "");
    ConsoleVariable windowWidth("r_width", 1024, "Current screen width.");
    ConsoleVariable windowHeight("r_height", 576, "Current screen height.");
    ConsoleVariable windowResize(ConsoleDefinition::Internal, false);
}

//
// Context
//

namespace
{
    bool                isInitialized = false;
    bool                isQuitting = false;
    std::string         workingDir = "";

    Logger              logger;
    LoggerOutputFile    loggerOutputFile;
    LoggerOutputConsole loggerOutputConsole;
    ConsoleSystem       consoleSystem;
    ConsoleHistory      consoleHistory;
    ConsoleFrame        consoleFrame;
    ShapeRenderer       shapeRenderer;
    TextRenderer        textRenderer;
    FrameCounter        frameCounter;

    Texture             textureBlank;

    SDL_Window*         systemWindow = nullptr;
    SDL_GLContext       graphicsContext = nullptr;
    FT_Library          fontLibrary = nullptr;
}

//
// Context Methods
//

bool Context::Initialize()
{
    assert(!isInitialized);

    //
    // Scope Guard
    //

    // Emergency cleanup call on failure.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!isInitialized)
        {
            Cleanup();
        }
    });

    //
    // Config
    //

    // Get the path to the asset directory.
    workingDir = GetTextFileContent("WorkingDir.txt");

    //
    // Logger Outputs
    //

    // Add a logger file output.
    if(!loggerOutputFile.Open("Log.txt"))
        return false;

    logger.AddOutput(&loggerOutputFile);

    // Add a logger console output.
    logger.AddOutput(&loggerOutputConsole);

    //
    // Console System
    //

    // Initialize the console system.
    if(!consoleSystem.Initialize())
        return false;

    // Register definitions created before the console system was initialized.
    ConsoleDefinition::RegisterStatic();

    //
    // Console History
    //

    // Initialize the console history.
    if(!consoleHistory.Initialize(32))
        return false;

    // Write a welcoming message.
    consoleHistory.Write("Welcome to developer's console!");

    //
    // Print system info after logger systems are up.
    //

    // Print the working directory path.
    Log() << "Working directory: \"" << workingDir << "\"";

    //
    // SDL
    //

    // Initialize SDL library.
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Log() << "Failed to initialize SDL library! Error: " << SDL_GetError();
        return false;
    }

    // Set requested attributes.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
 
    // Create a window.
    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    if(Console::windowResize.GetBool())
    {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    systemWindow = SDL_CreateWindow(
        Console::windowName.GetString().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Console::windowWidth.GetInteger(),
        Console::windowHeight.GetInteger(),
        windowFlags
    );

    Log() << "Created " << Console::windowWidth.GetInteger() << "x" << Console::windowHeight.GetInteger() << " window.";

    if(systemWindow == nullptr)
    {
        Log() << "Failed to create a window! Error: " << SDL_GetError();
        return false;
    }

    // For some reason text input is enabled by default.
    SDL_StopTextInput();

    //
    // OpenGL
    //

    // Create OpenGL context.
    graphicsContext = SDL_GL_CreateContext(systemWindow);

    if(graphicsContext == nullptr)
    {
        Log() << "Failed to create OpenGL context! Error: " << SDL_GetError();
        return false;
    }

    // Check the version of the created context.
    int versionMajor, versionMinor;

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &versionMajor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &versionMinor);

    Log() << "Created OpenGL " << versionMajor << "." << versionMinor << " context.";

    //
    // GLEW
    //

    // Initialize GLEW library.
    glewExperimental = GL_TRUE;

    GLenum glewError = glewInit();

    if(glewError != GLEW_OK)
    {
        Log() << "Failed to initialize GLEW library! Error: " << glewGetErrorString(glewError);
        return false;
    }

    //
    // FreeType
    //

    // Initialize FreeType library.
    if(FT_Init_FreeType(&fontLibrary) != 0)
    {
        Log() << "Failed to initialize FreeType library!";
        return false;
    }

    //
    // Blank Texture
    //

    // Create blank white texture.
    unsigned char* textureBlankData[2 * 2 * 4];
    memset(&textureBlankData[0], 255, sizeof(unsigned char) * 4 * 2 * 2);

    if(!textureBlank.Initialize(2, 2, GL_RGBA, textureBlankData))
    {
        Log() << "Failed to initialize a blank texture!";
        return false;
    }

    //
    // Shape Renderer
    //

    // Initialize the shape renderer.
    if(!shapeRenderer.Initialize(128))
        return false;

    //
    // Text Renderer
    //

    // Initialize the text renderer.
    if(!textRenderer.Initialize(64))
        return false;

    //
    // Console Frame
    //

    // Initialize the console frame.
    if(!consoleFrame.Initialize())
        return false;

    //
    // Frame Counter
    //

    // Initialize the frame counter.
    if(!frameCounter.Initialize(30))
        return false;

    //
    // Success!
    //

    // Set initialized state.
    isInitialized = true;

    return true;
}

void Context::Cleanup()
{
    Log() << "Cleaning up...";

    //
    // Frame Counter
    //

    frameCounter.Cleanup();

    //
    // Console Frame
    //

    consoleFrame.Cleanup();

    //
    // Graphics
    //

    textRenderer.Cleanup();
    shapeRenderer.Cleanup();
    textureBlank.Cleanup();

    //
    // FreeType
    //

    FT_Done_FreeType(fontLibrary);
    fontLibrary = nullptr;

    //
    // SDL
    //

    SDL_GL_DeleteContext(graphicsContext);
    graphicsContext = nullptr;

    SDL_DestroyWindow(systemWindow);
    systemWindow = nullptr;

    SDL_Quit();

    //
    // Console
    //

    consoleHistory.Cleanup();
    consoleSystem.Cleanup();

    //
    // Logger
    //

    logger.Cleanup();
    loggerOutputFile.Cleanup();
    loggerOutputConsole.Cleanup();

    //
    // Context
    //

    workingDir = "";
    isQuitting = false;
    isInitialized = false;
}

void Context::Quit()
{
    if(!isInitialized)
        return;

    isQuitting = true;
}

//
// Context Accessors
//

bool Context::IsInitialized()
{
    return isInitialized;
}

bool Context::IsQuitting()
{
    return isQuitting;
}

std::string Context::WorkingDir()
{
    return workingDir;
}

Logger& Context::Logger()
{
    return logger;
}

ConsoleSystem& Context::ConsoleSystem()
{
    return consoleSystem;
}

ConsoleHistory& Context::ConsoleHistory()
{
    return consoleHistory;
}

ConsoleFrame& Context::ConsoleFrame()
{
    return consoleFrame;
}

ShapeRenderer& Context::ShapeRenderer()
{
    return shapeRenderer;
}

TextRenderer& Context::TextRenderer()
{
    return textRenderer;
}

FrameCounter& Context::FrameCounter()
{
    return frameCounter;
}

Texture& Context::TextureBlank()
{
    return textureBlank;
}

SDL_Window* Context::SystemWindow()
{
    return systemWindow;
}

SDL_GLContext Context::GraphicsContext()
{
    return graphicsContext;
}

FT_Library Context::FontLibrary()
{
    return fontLibrary;
}
