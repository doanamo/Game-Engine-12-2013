#include "Precompiled.hpp"
#include "Context.hpp"

std::string Context::workingDir = "";

Logger*			Context::logger			= nullptr;
ConsoleSystem*	Context::consoleSystem	= nullptr;
ConsoleFrame*	Context::consoleFrame	= nullptr;
TextRenderer*	Context::textRenderer	= nullptr;

FT_Library Context::Private::fontLibrary = nullptr;
