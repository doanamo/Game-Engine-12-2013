#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"

ConsoleFrame::ConsoleFrame()
{
}

ConsoleFrame::~ConsoleFrame()
{
	Shutdown();
}

bool ConsoleFrame::Initialize()
{
	Shutdown();

	return true;
}

void ConsoleFrame::Shutdown()
{
}

void ConsoleFrame::Draw()
{
}
