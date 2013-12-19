#pragma once

#include "Precompiled.hpp"

//
// Context
//

struct Context
{
	static std::string workingDir;

	static class Logger*		logger;
	static class ConsoleSystem*	consoleSystem;
	static class ConsoleFrame*	consoleFrame;

	struct Private
	{
		static FT_Library fontLibrary;
	};
};
