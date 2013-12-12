#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"

ConsoleFrame::ConsoleFrame()
{
}

ConsoleFrame::~ConsoleFrame()
{
	Cleanup();
}

bool ConsoleFrame::Initialize()
{
	Cleanup();

	return true;
}

void ConsoleFrame::Cleanup()
{
}

void ConsoleFrame::Draw()
{
}
