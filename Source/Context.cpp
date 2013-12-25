#include "Precompiled.hpp"
#include "Context.hpp"

std::string Context::workingDir = "";

Logger*			Context::logger			= nullptr;
ConsoleSystem*	Context::consoleSystem	= nullptr;
ConsoleHistory*	Context::consoleHistory	= nullptr;
ConsoleFrame*	Context::consoleFrame	= nullptr;
ShapeRenderer*	Context::shapeRenderer	= nullptr;
TextRenderer*	Context::textRenderer	= nullptr;

Texture* Context::textureBlank = nullptr;

FT_Library Context::Private::fontLibrary = nullptr;
