#include "Precompiled.hpp"
#include "Context.hpp"

std::string Context::workingDir = "";

Logger*			Context::logger			= nullptr;
ConsoleSystem*	Context::consoleSystem	= nullptr;
ConsoleFrame*	Context::consoleFrame	= nullptr;
