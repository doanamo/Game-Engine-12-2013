#include "Precompiled.hpp"

#include "Logger/LoggerOutputFile.hpp"
#include "Logger/LoggerOutputConsole.hpp"

#include "Console/ConsoleSystem.hpp"
#include "Console/ConsoleHistory.hpp"
#include "Console/ConsoleFrame.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexInput.hpp"
#include "Graphics/Texture.hpp"

#include "ShapeRenderer.hpp"

#include "Font.hpp"
#include "TextRenderer.hpp"

//
// Console Commands
//

void CommandHelp(std::string arguments)
{
	// Get some more help here.
	if(arguments.empty())
	{
		Log() << "Type \"help <definition>\" for more info on a particular command or variable.";
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
		Log() << "Couldn't find \"" << name << "\" definition.";
		return;
	}

	// Print the description.
	Log() << definition->GetDescription();
}

void CommandEcho(std::string arguments)
{
	Log() << arguments;
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

	// Make instance current.
	Context::logger = &logger;

	SCOPE_GUARD(Context::logger = nullptr);

	//
	// Logger Outputs
	//

	// Create and add file output.
	LoggerOutputFile outputFile("Log.txt");

	logger.AddOutput(&outputFile);

	SCOPE_GUARD(logger.RemoveOutput(&outputFile));

	// Create and add console output.
	LoggerOutputConsole outputConsole;

	logger.AddOutput(&outputConsole);

	SCOPE_GUARD(logger.RemoveOutput(&outputConsole));

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
	// Console History
	//

	// Initialize the console history.
	ConsoleHistory consoleHistory;
	if(!consoleHistory.Initialize(4096))
		return -1;

	consoleHistory.Write("Welcome to developer's console!");

	// Make instance current.
	Context::consoleHistory = &consoleHistory;

	SCOPE_GUARD(Context::consoleHistory = nullptr);

	//
	// Print configuration.
	//

	// Print the working directory path.
	Log() << "Working directory: \"" << Context::workingDir << "\"";

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

	// For some reason text input is enabled by default.
	SDL_StopTextInput();

	//
	// OpenGL
	//

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

	// Make instance current.
	Context::Private::fontLibrary = fontLibrary;

	SCOPE_GUARD(Context::Private::fontLibrary = nullptr);

	//
	// Blank Texture
	//

	// Create blank white texture.
	unsigned char* textureBlankData[2 * 2 * 4];
	memset(&textureBlankData[0], 255, sizeof(unsigned char) * 4 * 2 * 2);

	Texture textureBlank;
	if(!textureBlank.Initialize(2, 2, GL_RGBA, textureBlankData))
		return -1;

	// Make instance current.
	Context::textureBlank = &textureBlank;

	SCOPE_GUARD(Context::textureBlank = nullptr);

	//
	// Font
	//

	// Load font file.
	Font font;
	if(!font.Load(Context::workingDir + "Data/Fonts/SourceSansPro.ttf", 22, 512, 512))
		return -1;
	
	// Cache ASCII character set.
	font.CacheASCII();

	//
	// Shape Renderer
	//

	// Initialize the shape renderer.
	ShapeRenderer shapeRenderer;
	if(!shapeRenderer.Initialize(8))
		return -1;

	// Make instance current.
	Context::shapeRenderer = &shapeRenderer;

	SCOPE_GUARD(Context::shapeRenderer = nullptr);

	//
	// Text Renderer
	//

	// Initialize the text renderer.
	TextRenderer textRenderer;
	if(!textRenderer.Initialize(32))
		return -1;

	// Make instance current.
	Context::textRenderer = &textRenderer;

	SCOPE_GUARD(Context::textRenderer = nullptr);

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
	// Test
	//

	// Projection.
	glm::mat4x4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);

	//
	// Example
	//

	if(argc >= 1)
	{
		userProfile.SetString(argv[0]);
	}

	Log() << mouseSensivity.GetName() << " = " << mouseSensivity.GetString();
	Log() << playerHealth.GetName() << " = " << playerHealth.GetString();
	Log() << playerAlive.GetName() << " = " << playerAlive.GetString();
	
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
					if(!Context::consoleFrame->IsOpen())
					{
						// Open console frame.
						Context::consoleFrame->Open();

						// Start text input.
						SDL_StartTextInput();
					}
					else
					{
						// Close console frame.
						Context::consoleFrame->Close();

						// End text input.
						SDL_StopTextInput();

						// Clear text input (temp).
						Context::consoleFrame->ClearInput();
					}
				}
				else
				if(event.key.keysym.scancode == SDL_SCANCODE_RETURN && event.key.repeat == 0)
				{
					if(Context::consoleFrame->IsOpen())
					{
						// Execute console input.
						Context::consoleFrame->ExecuteInput();
					}
				}
				break;

			case SDL_TEXTINPUT:
				if(Context::consoleFrame->IsOpen())
				{
					Context::consoleFrame->AppendInput(event.text.text);
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

		// Draw text.
		glm::vec2 textPosition(10.0f, 576.0f - 10.0f);
		float textWidth = 800.0f;

		const char* text0 = 
			"Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. "
			"Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, "
			"ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, "
			"fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, "
			"justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper "
			"nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. "
			"Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius "
			"laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies "
			"nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, "
			"sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit vel, luctus pulvinar, hendrerit id, lorem. "
			"Maecenas nec odio et ante tincidunt tempus. Donec vitae sapien ut libero venenatis faucibus. Nullam quis ante. "
			"Etiam sit amet orci eget eros faucibus tincidunt. Duis leo. Sed fringilla mauris sit amet nibh. Donec sodales "
			"sagittis magna. Sed consequat, leo eget bibendum sodales, augue velit cursus nunc.";

		Context::textRenderer->SetDebug(true);
		Context::textRenderer->Draw(&font, textPosition, textWidth, projection, text0);
		Context::textRenderer->SetDebug(false);

		// Draw console frame.
		Context::consoleFrame->Draw(projection);

		// Present the window content.
		SDL_GL_SetSwapInterval(0);
		SDL_GL_SwapWindow(window);
	}

	return 0;
}
