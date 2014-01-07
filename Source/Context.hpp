#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class Logger;
class ConsoleSystem;
class ConsoleHistory;
class ConsoleFrame;
class ShapeRenderer;
class TextRenderer;
class FrameCounter;
class Texture;

//
// Console Definitions
//

namespace Console
{
    extern ConsoleVariable windowName;
    extern ConsoleVariable windowWidth;
    extern ConsoleVariable windowHeight;
    extern ConsoleVariable windowResize;
}

//
// Context
//

namespace Context
{
    // Context methods.
    bool Initialize();
    void Cleanup();

    void Quit();

    // Context accessors.
    bool            IsInitialized();
    bool            IsQuitting();
    std::string     WorkingDir();

    Logger&         Logger();
    ConsoleSystem&  ConsoleSystem();
    ConsoleHistory& ConsoleHistory();
    ConsoleFrame&   ConsoleFrame();
    ShapeRenderer&  ShapeRenderer();
    TextRenderer&   TextRenderer();
    FrameCounter&   FrameCounter();

    Texture&        TextureBlank();

    SDL_Window*     SystemWindow();
    SDL_GLContext   GraphicsContext();
    FT_Library      FontLibrary();
}
