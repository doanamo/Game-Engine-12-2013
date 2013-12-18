#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Texture.hpp"

//
// Flip Surface
//

void FlipSurface(SDL_Surface* surface)
{
	assert(surface != nullptr);

	size_t surfaceRowSize = surface->pitch;
	size_t surfaceRowLast = surface->pitch * (surface->h - 1);

	// Create temporary row buffer.
	unsigned char* rowBuffer = new unsigned char[surfaceRowSize];

	SCOPE_GUARD(delete[] rowBuffer);

	// Flip surface line by line.
	SDL_LockSurface(surface);

	unsigned char* surfaceData = reinterpret_cast<unsigned char*>(surface->pixels);

	for(long i = 0; i < surface->h / 2; ++i)
	{
		unsigned long offset = i * surface->pitch;

		// Copy top to temporary.
		memcpy(&rowBuffer[0], &surfaceData[offset], surfaceRowSize);

		// Copy bottom to top.
		memcpy(&surfaceData[offset], &surfaceData[surfaceRowLast - offset], surfaceRowSize);

		// Copy temporary to bottom.
		memcpy(&surfaceData[surfaceRowLast - offset], &rowBuffer[0], surfaceRowSize);
	}

	SDL_UnlockSurface(surface);
}

//
// Console Commands
//

void CommandHelp(std::string arguments)
{
	// Get some more help here.
	if(arguments.empty())
	{
		std::cout << "Type \"help <definition>\" for more info on a particular command or variable." << std::endl;
		return;
	}

	// Locate the first white space.
	std::size_t separator = arguments.find(' ');

	// Get the first argument.
	std::string name(arguments);

	if(separator != std::string::npos)
	{
		name.erase(separator);
	}

	// Get the definition.
	ConsoleDefinition* definition = Context::consoleSystem->FindDefinition(name);

	if(definition == nullptr)
	{
		std::cout << "Couldn't find \"" << name << "\" definition." << std::endl;
		return;
	}

	// Print the description.
	std::cout << definition->GetDescription() << std::endl;
}

void CommandEcho(std::string arguments)
{
	std::cout << arguments << std::endl;
}

bool isQuitting = false;

void CommandQuit(std::string arguments)
{
	isQuitting = true;
}

//
// Console Definitions
//

ConsoleCommand help("help", &CommandHelp, "Prints a description of a command or a variable.");
ConsoleCommand echo("echo", &CommandEcho, "Prints provided arguments in the console.");
ConsoleCommand quit("quit", &CommandQuit, "Quits the application.");

//ConsoleVariable windowWidth("r_width", "1920", "Window width.");
//ConsoleVariable windowHeight("r_height", "1024", "Window height.");
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
	// Config
	//

	// Get the path to the asset directory.
	Context::workingDir = GetTextFileContent("WorkingDir.txt");

	//
	// Logger
	//

	// Create a logger.
	Logger logger;

	if(!logger.Open("Log.txt"))
		return -1;

	// Make instance current.
	Context::logger = &logger;

	SCOPE_GUARD(Context::logger = nullptr);

	// Print the working directory path.
	Log() << "Working directory: \"" << Context::workingDir << "\"";

	//
	// Console System
	//

	// Initialize the console system.
	ConsoleSystem consoleSystem;
	if(!consoleSystem.Initialize())
		return -1;

	SCOPE_GUARD(consoleSystem.Cleanup());

	// Make instance current.
	Context::consoleSystem = &consoleSystem;

	SCOPE_GUARD(Context::consoleSystem = nullptr);

	// Register definitions created before the console system was initialized.
	ConsoleDefinition::RegisterStatic();

	//
	// SDL
	//

	// Initialize SDL library.
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Log() << "Failed to initialize SDL library! Error: " << SDL_GetError();
		return -1;
	}

	SCOPE_GUARD(SDL_Quit());

	// Set requested attributes.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
	// Create a window.
	int windowWidth = 1024;
	int windowHeight = 576;

	SDL_Window* window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_OPENGL
	);

	if(window == nullptr)
	{
		Log() << "Failed to create a window! Error: " << SDL_GetError();
		return -1;
	}

	SCOPE_GUARD(SDL_DestroyWindow(window));

	// Create OpenGL context.
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if(glContext == nullptr)
	{
		Log() << "Failed to create OpenGL context! Error: " << SDL_GetError();
		return -1;
	}

	SCOPE_GUARD(SDL_GL_DeleteContext(glContext));

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
		return -1;
	}

	//
	// FreeType
	//

	// Initialize FreeType library.
	FT_Library fontLibrary;

	if(FT_Init_FreeType(&fontLibrary) != 0)
	{
		Log() << "Failed to initialize FreeType library!";
		return -1;
	}

	SCOPE_GUARD(FT_Done_FreeType(fontLibrary));

	//
	// Test
	//

	// Load a font file.
	FT_Face fontFace;

	if(FT_New_Face(fontLibrary, std::string(Context::workingDir + "Data/Fonts/SourceSansPro.ttf").c_str(), 0, &fontFace) != 0)
		return -1;

	SCOPE_GUARD(FT_Done_Face(fontFace));

	// Set font encoding.
	if(FT_Select_Charmap(fontFace, FT_ENCODING_UNICODE) != 0)
		return -1;

	// Set font size.
	if(FT_Set_Char_Size(fontFace, 0, 64 * 64, 72, 72) != 0)
		return -1;

	// Create a text surface.
	int textSurfaceWidth = 1024;
	int textSurfaceHeight = 128;

	SDL_Surface* textSurface = SDL_CreateRGBSurface(0, textSurfaceWidth, textSurfaceHeight, 8, 0, 0, 0, 0);

	if(textSurface == nullptr)
		return -1;

	SCOPE_GUARD(SDL_FreeSurface(textSurface));

	// Draw text glyphs.
	wchar_t* text = L"Hello world! Test: œŸ³óæ¹ê€³æ :)";

	glm::ivec2 drawPosition(0, 64);

	for(std::size_t i = 0; i < std::wcslen(text); ++i)
	{
		// Load font glyph.
		FT_ULong glyphIndex = FT_Get_Char_Index(fontFace, text[i]);

		if(FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT) != 0)
			return -1;

		FT_GlyphSlot glyphSlot = fontFace->glyph;

		// Render font glyph.
		if(FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL) != 0)
			return -1;

		FT_Bitmap* glyphBitmap = &glyphSlot->bitmap;

		// Create a glyph surface.
		SDL_Surface* glyphSurface = SDL_CreateRGBSurface(0, glyphBitmap->width, glyphBitmap->rows, 8, 0, 0, 0, 0);

		if(glyphSurface == nullptr)
			return -1;

		SCOPE_GUARD(SDL_FreeSurface(glyphSurface));

		// Copy glyph pixels.
		SDL_LockSurface(glyphSurface);

		unsigned char* glyphSurfaceData = reinterpret_cast<unsigned char*>(glyphSurface->pixels);

		for(long i = 0; i < glyphBitmap->rows; ++i)
		{
			memcpy(&glyphSurfaceData[i * glyphSurface->pitch], &glyphBitmap->buffer[i * glyphBitmap->pitch], glyphBitmap->width);
		}

		SDL_UnlockSurface(glyphSurface);

		// Draw glyph surface.
		SDL_Rect rect;
		rect.x = drawPosition.x + glyphSlot->bitmap_left;
		rect.y = drawPosition.y - glyphSlot->bitmap_top;
		rect.w = glyphBitmap->width;
		rect.h = glyphBitmap->rows;

		SDL_BlitSurface(glyphSurface, nullptr, textSurface, &rect);

		// Advance drawing position.
		drawPosition.x += glyphSlot->advance.x >> 6;
		drawPosition.y += glyphSlot->advance.y >> 6;
	}

	// Flip surface.
	FlipSurface(textSurface);

	//
	// Test
	//

	// Projection.
	glm::mat4x4 proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

	// Shader.
	Shader textShader;
	if(!textShader.Load(Context::workingDir + "Data/Shaders/Text.glsl"))
		return -1;

	// Vertex buffer.
	struct textVertex
	{
		glm::vec2 position;
		glm::vec2 texture;
	};

	textVertex textBuffer[] =
	{
		{ glm::vec2(            0.0f,              0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec2(textSurfaceWidth,              0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec2(textSurfaceWidth, textSurfaceHeight), glm::vec2(1.0f, 1.0f) },

		{ glm::vec2(            0.0f,              0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec2(textSurfaceWidth, textSurfaceHeight), glm::vec2(1.0f, 1.0f) },
		{ glm::vec2(            0.0f, textSurfaceHeight), glm::vec2(0.0f, 1.0f) },
	};
	
	VertexBuffer textVertexBuffer;
	if(!textVertexBuffer.Initialize(sizeof(textVertex), StaticArraySize(textBuffer), &textBuffer[0]))
		return -1;

	// Vertex input.
	VertexAttribute vertexAttributes[] =
	{
		{ 0, &textVertexBuffer, VertexAttributeTypes::Float2 },
		{ 1, &textVertexBuffer, VertexAttributeTypes::Float2 },
	};

	VertexInput textVertexInput;
	if(!textVertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
		return -1;

	// Texture.
	Texture textTexture;
	if(!textTexture.Initialize(textSurfaceWidth, textSurfaceHeight, GL_RED, textSurface->pixels))
		return -1;

	//
	// Console Frame
	//

	// Initialize the console frame.
	ConsoleFrame consoleFrame;
	if(!consoleFrame.Initialize())
		return -1;

	SCOPE_GUARD(consoleFrame.Cleanup());

	// Make instance current.
	Context::consoleFrame = &consoleFrame;

	SCOPE_GUARD(Context::consoleFrame = nullptr);

	//
	// Example
	//

	if(argc >= 1)
	{
		userProfile.SetString(argv[0]);
	}

	//std::cout << windowWidth.GetName() << " = " << windowWidth.GetString() << std::endl;
	//std::cout << windowHeight.GetName() << " = " << windowHeight.GetString() << std::endl;
	std::cout << mouseSensivity.GetName() << " = " << mouseSensivity.GetString() << std::endl;
	std::cout << playerHealth.GetName() << " = " << playerHealth.GetString() << std::endl;
	std::cout << playerAlive.GetName() << " = " << playerAlive.GetString() << std::endl;
	
	//
	// Main Loop
	//

	while(!isQuitting)
	{
		// Handle window events.
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				isQuitting = true;
				break;

			case SDL_KEYDOWN:
				if(event.key.keysym.scancode == SDL_SCANCODE_GRAVE && event.key.repeat == 0)
				{
					Context::consoleFrame->Toggle();
				}
				break;
			}
		}

		// Setup the viewport.
		int windowWidth, windowHeight;
		SDL_GetWindowSize(window, &windowWidth, &windowHeight);

		glViewport(0, 0, windowWidth, windowHeight);

		// Clear the screen.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw console frame.
		Context::consoleFrame->Draw();
		
		// Draw text.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textTexture.GetHandle());

		glUseProgram(textShader.GetHandle());
		glBindVertexArray(textVertexInput.GetHandle());

		glm::mat4 world = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (float)windowHeight - textSurfaceHeight, 0.0f));

		glUniformMatrix4fv(textShader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(proj * world));
		glUniform1i(textShader.GetUniform("texture"), 0);

		glDrawArrays(GL_TRIANGLES, 0, textVertexBuffer.GetElementCount());

		glBindVertexArray(0);
		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_BLEND);

		// Present the window content.
		SDL_GL_SetSwapInterval(0);
		SDL_GL_SwapWindow(window);

		// Handle console input.
		/*
		std::cout << "> ";

		std::string input;
		std::getline(std::cin, input);

		if(!input.empty())
		{
			Context::consoleSystem->Execute(input);
		}
		*/
	}

	return 0;
}
