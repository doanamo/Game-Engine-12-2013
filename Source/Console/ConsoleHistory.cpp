#include "Precompiled.hpp"
#include "ConsoleHistory.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a console history! "
}

ConsoleHistory::ConsoleHistory() :
	m_outputHistorySize(0),
	m_initialized(false)
{
}

ConsoleHistory::~ConsoleHistory()
{
	Cleanup();
}

bool ConsoleHistory::Initialize(int outputHistorySize)
{
	Cleanup();

	// Validate arguments.
	if(outputHistorySize <= 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"outputHistorySize\" is invalid.";
		return false;
	}

	m_outputHistorySize = outputHistorySize;

	// Success!
	m_initialized = true;

	return true;
}

void ConsoleHistory::Cleanup()
{
	ClearContainer(m_outputHistory);
	m_outputHistorySize = 0;

	m_initialized = false;
}

void ConsoleHistory::Write(const char* text)
{
	if(!m_initialized)
		return;

	if(text == nullptr)
		return;

	// Check if there is space for another string.
	assert((int)m_outputHistory.size() <= m_outputHistorySize);

	if((int)m_outputHistory.size() == m_outputHistorySize)
	{
		// Remove oldest string.
		m_outputHistory.pop_back();
	}

	// Add string to the history.
	m_outputHistory.emplace_front(text);
}

void ConsoleHistory::Clear()
{
	if(!m_initialized)
		return;

	// Clear output strings.
	ClearContainer(m_outputHistory);
}

const char* ConsoleHistory::GetText(int index) const
{
	if(IsEmpty())
		return nullptr;

	if(index < 0)
		return nullptr;

	if(index >= (int)m_outputHistory.size())
		return nullptr;

	// Return string at index.
	return m_outputHistory[index].c_str();
}
