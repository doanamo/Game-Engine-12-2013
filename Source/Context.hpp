#pragma once

#include "Precompiled.hpp"

//
// Context
//

struct Context
{
	static bool isQuitting;

	static std::string workingDir;

	static class Logger*			logger;
	static class ConsoleSystem*		consoleSystem;
	static class ConsoleHistory*	consoleHistory;
	static class ConsoleFrame*		consoleFrame;
	static class ShapeRenderer*		shapeRenderer;
	static class TextRenderer*		textRenderer;

	static class Texture* textureBlank;

	struct Private
	{
		static FT_Library fontLibrary;
	};
};
