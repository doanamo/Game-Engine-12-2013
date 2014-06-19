#include "Precompiled.hpp"
#include "MainContext.hpp"

#include "Logger/Logger.hpp"
#include "Logger/LoggerOutputFile.hpp"
#include "Logger/LoggerOutputConsole.hpp"
#include "System/BaseFrame.hpp"
#include "System/FrameCounter.hpp"
#include "System/CacheManager.hpp"
#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleHistory.hpp"
#include "Console/ConsoleFrame.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/CoreRenderer.hpp"
#include "Graphics/ShapeRenderer.hpp"
#include "Graphics/TextRenderer.hpp"
#include "Game/MainFrame.hpp"

//
// Console Variables
//

namespace Console
{
    // Window settings.
    ConsoleVariable windowName(ConsoleDefinition::Internal, "");
    ConsoleVariable windowWidth("r_width", 1024, "Current screen width.");
    ConsoleVariable windowHeight("r_height", 576, "Current screen height.");
    ConsoleVariable windowResize(ConsoleDefinition::Internal, false);

    // Render settings.
    ConsoleVariable renderVsync("r_vsync", true, "Enables vertical synchronization.");
}

//
// Context Data
//

namespace
{
    bool                isInitialized = false;
    bool                isQuitting = false;

    std::string         currentDir;
    std::string         sourceDir;
    std::string         workingDir;
    std::string         cacheDir;

    Logger              logger;
    LoggerOutputFile    loggerOutputFile;
    LoggerOutputConsole loggerOutputConsole;
    CacheManager        cacheManager;
    ConsoleSystem       consoleSystem;
    ConsoleHistory      consoleHistory;
    ConsoleFrame        consoleFrame;
    ScreenSpace         screenSpace;
    CoreRenderer        coreRenderer;
    ShapeRenderer       shapeRenderer;
    TextRenderer        textRenderer;
    FrameCounter        frameCounter;

    Texture             blankTexture;
    Font                defaultFont;

    SDL_Window*         systemWindow = nullptr;
    SDL_GLContext       graphicsContext = nullptr;
    FT_Library          fontLibrary = nullptr;

    MainFrame           mainFrame;
}

//
// Context Methods
//

bool Main::Initialize()
{
    assert(!isInitialized);

    //
    // Scope Guard
    //

    // Emergency cleanup call on failure.
    auto emergenyCleanup = MakeScopeGuard([&]()
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

    // Get the current directory.
    boost::filesystem::path currentDirPath = boost::filesystem::current_path();

    currentDir = currentDirPath.generic_string() + '/';

    // Get the source directory.
    boost::filesystem::path sourceDirPath = GetTextFileContent("SourceDir.txt");

    if(!sourceDirPath.empty())
    {
        sourceDir = sourceDirPath.generic_string() + '/';
    }

    // Get the working directory.
    boost::filesystem::path workingDirPath = GetTextFileContent("WorkingDir.txt");

    if(!workingDirPath.empty())
    {
        workingDir = workingDirPath.generic_string() + '/';
    }
    else
    {
        workingDir = currentDir;
    }

    // Get the cache directory.
    cacheDir = workingDir + "Cache/";

    //
    // Logger
    //

    // Set logger instance as global.
    Logger::SetGlobal(&logger);

    // Add default logger file output.
    if(loggerOutputFile.Open("Log.txt"))
    {
        logger.AddOutput(&loggerOutputFile);
    }

    //
    // Console System
    //

    // Initialize the console system.
    if(!consoleSystem.Initialize())
        return false;

    //
    // Console History
    //

    // Initialize the console history.
    if(!consoleHistory.Initialize(128, 32))
        return false;

    // Write a welcoming message.
    consoleHistory.WriteOutput("Welcome to developer's console!");

    // Add a logger console output that writes to history.
    logger.AddOutput(&loggerOutputConsole);

    // Write to console output if for some reason logger file output failed to open.
    if(!loggerOutputFile.IsOpen())
    {
        consoleHistory.WriteOutput("Failed to open logger file output!");
    }

    //
    // Print system info after logger systems are up.
    //

    // Print directory paths.
    Log() << "Current directory: \"" << currentDir << "\"";
    Log() << "Working directory: \"" << workingDir << "\"";
    Log() << "Cache directory: \"" << cacheDir << "\"";

    //
    // System
    //

    // Initialize the cache manager.
    if(!cacheManager.Initialize())
        return false;

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
    SDL_GL_SetAttribute(SDL_GL_STEREO, 0);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
 
    // Create a window.
    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

    if(Console::windowResize)
    {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }

    systemWindow = SDL_CreateWindow(
        Console::windowName,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Console::windowWidth,
        Console::windowHeight,
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

    if(!blankTexture.Initialize(2, 2, GL_RGBA, textureBlankData))
    {
        Log() << "Failed to initialize a blank texture!";
        return false;
    }

    //
    // Default Font
    //

    // Load a default font.
    if(!defaultFont.Load("Data/Fonts/SourceSansPro.ttf"))
        return false;
    
    // Cache ASCII character set.
    defaultFont.CacheASCII();

    //
    // Core Renderer
    //

    // Initialize the core renderer.
    if(!coreRenderer.Initialize())
        return false;

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
    // Main Frame
    //

    // Initialize the main frame.
    if(!mainFrame.Initialize())
        return false;

    //
    // Success!
    //

    isInitialized = true;

    return true;
}

void Main::Cleanup()
{
    Log() << "Cleaning up the main context...";

    //
    // Main Frame
    //

    mainFrame.Cleanup();

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

    screenSpace.Cleanup();

    textRenderer.Cleanup();
    shapeRenderer.Cleanup();
    coreRenderer.Cleanup();

    blankTexture.Cleanup();
    defaultFont.Cleanup();

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
    // System
    //

    cacheManager.Cleanup();

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

    Logger::SetGlobal(nullptr);

    //
    // Context
    //

    currentDir = "";
    sourceDir = "";
    workingDir = "";
    cacheDir = "";

    isQuitting = false;
    isInitialized = false;
}

void Main::Quit()
{
    if(!isInitialized)
        return;

    isQuitting = true;
}

//
// Context Accessors
//

bool Main::IsInitialized()
{
    return isInitialized;
}

bool Main::IsQuitting()
{
    return isQuitting;
}

std::string Main::CurrentDir()
{
    return currentDir;
}

std::string Main::SourceDir()
{
    return sourceDir;
}

std::string Main::WorkingDir()
{
    return workingDir;
}

std::string Main::CacheDir()
{
    return cacheDir;
}

Logger& Main::Logger()
{
    return logger;
}

CacheManager& Main::CacheManager()
{
    return cacheManager;
}

ConsoleSystem& Main::ConsoleSystem()
{
    return consoleSystem;
}

ConsoleHistory& Main::ConsoleHistory()
{
    return consoleHistory;
}

ConsoleFrame& Main::ConsoleFrame()
{
    return consoleFrame;
}

ScreenSpace& Main::ScreenSpace()
{
    return screenSpace;
}

CoreRenderer& Main::CoreRenderer()
{
    return coreRenderer;
}

ShapeRenderer& Main::ShapeRenderer()
{
    return shapeRenderer;
}

TextRenderer& Main::TextRenderer()
{
    return textRenderer;
}

FrameCounter& Main::FrameCounter()
{
    return frameCounter;
}

Texture& Main::BlankTexture()
{
    return blankTexture;
}

Font& Main::DefaultFont()
{
    return defaultFont;
}

SDL_Window* Main::SystemWindow()
{
    return systemWindow;
}

SDL_GLContext Main::GraphicsContext()
{
    return graphicsContext;
}

FT_Library Main::FontLibrary()
{
    return fontLibrary;
}

BaseFrame& Main::MainFrame()
{
    return mainFrame;
}
