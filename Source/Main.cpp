#include "Precompiled.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"

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

ConsoleVariable windowWidth("r_width", "1920", "Window width.");
ConsoleVariable windowHeight("r_height", "1024", "Window height.");
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

	// Get the path to the working directory.
	Context::workingDir = GetTextFileContent("working_dir.txt");

	//
	// Logger
	//

	// Create a logger.
	Logger logger;

	if(!logger.Open(Context::workingDir + "Log.txt"))
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
	SDL_Window* window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1024,
		576,
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

	if(FT_New_Face(fontLibrary, std::string(Context::workingDir + "Data/SourceSansPro-Regular.ttf").c_str(), 0, &fontFace) != 0)
		return -1;

	SCOPE_GUARD(FT_Done_Face(fontFace));

	// Set font encoding.
	if(FT_Select_Charmap(fontFace, FT_ENCODING_UNICODE) != 0)
		return -1;

	// Set font size.
	if(FT_Set_Char_Size(fontFace, 0, 32 * 64, 72, 72) != 0)
		return -1;

	// Load font glyph.
	FT_ULong glyphIndex = FT_Get_Char_Index(fontFace, L'Œ');

	if(FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT) != 0)
		return -1;

	// Render font glyph.
	if(FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL) != 0)
		return -1;

	FT_Bitmap* glyphBitmap = &fontFace->glyph->bitmap;

	// Copy bitmap to SDL surface and save to a file.
	SDL_Surface* glyphSurface = SDL_CreateRGBSurface(0, glyphBitmap->width, glyphBitmap->rows, 32, 0, 0, 0, 0);

	assert(glyphSurface != nullptr);

	SDL_LockSurface(glyphSurface);

	unsigned char* glyphSurfaceData = reinterpret_cast<unsigned char*>(glyphSurface->pixels);

	for(long i = 0; i < glyphBitmap->width * glyphBitmap->rows; ++i)
	{
		glyphSurfaceData[i * 4 + 0] = glyphBitmap->buffer[i];
		glyphSurfaceData[i * 4 + 1] = glyphBitmap->buffer[i];
		glyphSurfaceData[i * 4 + 2] = glyphBitmap->buffer[i];
		glyphSurfaceData[i * 4 + 3] = 255;
	}

	SDL_UnlockSurface(glyphSurface);

	SDL_SaveBMP(glyphSurface, std::string(Context::workingDir + "glyph.bmp").c_str());

	SDL_FreeSurface(glyphSurface);

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

	std::cout << windowWidth.GetName() << " = " << windowWidth.GetString() << std::endl;
	std::cout << windowHeight.GetName() << " = " << windowHeight.GetString() << std::endl;
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
