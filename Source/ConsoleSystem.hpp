#pragma once

#include "Precompiled.hpp"
#include "ConsoleDefinition.hpp"
#include "ConsoleCommand.hpp"
#include "ConsoleVariable.hpp"

//
// Console System
//

class ConsoleSystem : public NonCopyable
{
public:
	ConsoleSystem();
	~ConsoleSystem();

	bool Initialize();
	void Shutdown();

	void Execute(std::string input);

	void RegisterDefinition(ConsoleDefinition* definition);
	void UnregisterDefinition(ConsoleDefinition* definition);

	ConsoleDefinition* FindDefinition(std::string name);

private:
	std::map<std::string, ConsoleDefinition*> m_definitions;
};
