#include "Precompiled.hpp"

#include "ConsoleSystem.hpp"
#include "ConsoleFrame.hpp"

#include "Graphics/Shader.hpp"

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
	SCOPE_GUARD(system("pause"));

	//
	// Config
	//

	std::string workingDir = GetTextFileContent("working_dir.txt");

	//
	// SDL
	//

	// Initialize SDL library.
	std::cout<< "Initializing SDL library..." << std::endl;

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Failed to initialize SDL library! Error: " << SDL_GetError() << std::endl;
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
	std::cout << "Creating a window..." << std::endl;

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
		std::cout << "Failed to create a window! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SCOPE_GUARD(SDL_DestroyWindow(window));

	// Create OpenGL context.
	std::cout << "Creating OpenGL context..." << std::endl;

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if(glContext == nullptr)
	{
		std::cout << "Failed to create OpenGL context! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SCOPE_GUARD(SDL_GL_DeleteContext(glContext));

	// Check the version of the created context.
	int versionMajor, versionMinor;

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &versionMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &versionMinor);

	std::cout << "Created OpenGL " << versionMajor << "." << versionMinor << " context." << std::endl;

	//
	// GLEW
	//

	// Initialize GLEW library.
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();

	if(glewError != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW library! Error: " << glewGetErrorString(glewError) << std::endl;
		return -1;
	}

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
	// OpenGL
	//

	// Vertex data.
	const float vertexData[9] =
	{
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	// Vertex buffer.
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, &vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	SCOPE_GUARD(glDeleteBuffers(1, &vertexBuffer));

	// Vertex input.
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	SCOPE_GUARD(glDeleteVertexArrays(1, &vertexArray));

	// Create a shader.
	Shader shader;

	if(!shader.Load(workingDir + "Data/shader.glsl"))
		return -1;

	//
	// Example
	//

	std::cout << std::endl;

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
			}
		}

		// Clear the screen.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw stuff.
		glUseProgram(shader.GetHandle());

		glBindVertexArray(vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

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
