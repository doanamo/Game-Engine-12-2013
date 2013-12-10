#include "Precompiled.hpp"
#include "ConsoleSystem.hpp"

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
ConsoleVariable userProfile("s_userprofile", "./", "Current's user profile directory.");

//
// Main
//

int main(int argc, char* argv[])
{
	SCOPE_GUARD(system("pause"));

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
 
	// Create SDL window.
	std::cout << "Creating SDL window..." << std::endl;

	SDL_Window* window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		1024,
		576,
		SDL_WINDOW_SHOWN
	);

	if(window == nullptr)
	{
		std::cout << "Failed to create SDL window! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SCOPE_GUARD(SDL_DestroyWindow(window));

	// Create SDL renderer.
	std::cout << "Creating SDL renderer..." << std::endl;

	SDL_Renderer* renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if(renderer == nullptr)
	{
		std::cout << "Failed to create SDL renderer! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SCOPE_GUARD(SDL_DestroyRenderer(renderer));

	//
	// Initialization
	//

	// Initialize the console system.
	ConsoleSystem console;

	if(!console.Initialize())
		return -1;

	SCOPE_GUARD(console.Shutdown());

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

		// Clear the output surface.
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Present the output surface.
		SDL_RenderPresent(renderer);

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
