#include "Precompiled.hpp"
#include "ConsoleHistory.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a console history! "
}

ConsoleHistory::ConsoleHistory() :
	m_buffer(nullptr),
	m_bufferSize(0),
	m_bufferStart(-1),
	m_bufferEnd(-1)
{
}

ConsoleHistory::~ConsoleHistory()
{
	Cleanup();
}

bool ConsoleHistory::Initialize(int bufferSize)
{
	Cleanup();

	// Validate arguments.
	if(bufferSize <= 0)
	{
		Log() << LogInitializeError() << "Invalid argument - \"bufferSize\" is invalid.";
		return false;
	}

	m_bufferSize = bufferSize;

	// Allocate buffer memory.
	m_buffer = new char[bufferSize];

	if(m_buffer == nullptr)
	{
		Log() << LogInitializeError() << "Couldn't allocate buffer memory.";
		Cleanup();
		return false;
	}

	m_initialized = true;

	return true;
}

void ConsoleHistory::Cleanup()
{
	delete[] m_buffer;
	m_buffer = nullptr;
	m_bufferSize = 0;

	m_bufferStart = -1;
	m_bufferEnd = -1;
}

void ConsoleHistory::Write(const char* text)
{
	if(!m_initialized)
		return;

	//
	// Calculate variables.
	//

	// Calculate text size.
	std::size_t textSize = strlen(text);

	// Calculate buffer position where we will write the text.
	int writeStart = m_bufferEnd + 1;

	// If the buffer end pointer was in the last position, move writting position to the beginning of the buffer.
	assert(writeStart <= m_bufferSize);
	
	if(writeStart == m_bufferSize)
	{
		writeStart = 0;
	}

	// Calculate write size and include the null character.
	int writeSize = textSize + 1;

	// Check if the text will fit in the buffer at all.
	if(writeSize > m_bufferSize)
		return;

	// We are gonna see if we have overwritten any text nodes.
	bool nodeOverwritten = false;
	bool fullOverwrite = false;

	//
	// Write text to the buffer.
	//

	// Position where the write will end (to be determined).
	int writeEnd;

	// Check if we can copy the entire text at once. If not, split it into two segments.
	if(writeStart + writeSize > m_bufferSize)
	{
		assert(writeStart != 0);

		// Calculate segment sizes.
		int firstSegmentSize = m_bufferSize - writeStart;
		int secondSegmentSize = writeSize - firstSegmentSize;

		// Calculate write end.
		writeEnd = secondSegmentSize - 1;

		// Check if we did a full overwrite (if the last overwritten character was another null).
		if(m_buffer[writeEnd] == '\0')
			fullOverwrite = true;

		// Write both text segments (include null character).
		memcpy(&m_buffer[writeStart], &text[0], sizeof(char) * firstSegmentSize);
		memcpy(&m_buffer[0], &text[firstSegmentSize], sizeof(char) * secondSegmentSize);

		// Check if we overwritten any nodes.
		if(writeStart <= m_bufferStart && m_bufferStart <= m_bufferSize - 1)
		{
			nodeOverwritten = true;
		}
		else
		if(0 <= m_bufferStart && m_bufferStart <= writeEnd)
		{
			nodeOverwritten = true;
		}
	}
	else
	{
		// Calculate write end.
		writeEnd = writeStart + writeSize - 1;

		// Check if we did a full overwrite (if the last overwritten character was another null).
		if(m_buffer[writeEnd] == '\0')
			fullOverwrite = true;

		// Write the entire text to buffer (include null character).
		memcpy(&m_buffer[writeStart], &text[0], sizeof(char) * writeSize);

		// Check if we overwritten any nodes.
		if(writeStart <= m_bufferStart && m_bufferStart <= writeEnd)
			nodeOverwritten = true;
	}

	// Move the buffer end to the write end (this is the position of new null character).
	m_bufferEnd = writeEnd;

	// Move the buffer start if its the first text we ever written.
	if(m_bufferStart == -1)
		m_bufferStart = 0;

	//
	// Discard text node we possibly overwritten.
	//

	// Close the node we partially overwritten.
	if(nodeOverwritten)
	{
		if(fullOverwrite)
		{
			m_bufferStart = writeEnd + 1;
		}

		// Search after the text (including null) we wrote.
		int searchPosition = writeEnd + 1;

		// Wrap the search position if it's past the buffer size.
		assert(searchPosition <= m_bufferSize);

		if(searchPosition == m_bufferSize)
		{
			searchPosition = 0;
		}
		
		// Find the null character and close the node.
		bool foundNull = false;

		while(!foundNull)
		{
			// If we already determined it was a full overwrite, use this position as a new buffer start.
			if(fullOverwrite)
				break;

			// Look for null character of the text string we overwritten.
			if(m_buffer[searchPosition] == '\0')
				foundNull = true;

			// Advance the search position.
			++searchPosition;

			// Wrap if needed.
			if(searchPosition == m_bufferSize)
			{
				searchPosition = 0;
			}
		}

		// Set new buffer start position.
		m_bufferStart = searchPosition;
	}
}

std::string ConsoleHistory::GetText(int index)
{
	if(IsEmpty())
		return nullptr;

	if(index < 0)
		return nullptr;

	// String containing text we will return.
	// We can't return a text node pointer because it could be split in half.
	// We could avoid this by making the ring buffer avoid splitting text data in two
	// by always storing a text without any breaks. If split would be needed, just move 
	// the string to the beginning of the buffer. This would result in wasted space 
	// at the end of the buffer, but that would be perfectly fine.
	std::string text;

	//
	// Search for text node by index.
	//

	int i = index;

	int searchPosition = m_bufferEnd - 1;
	int textSize = 0;

	while(true)
	{
		// Checks if it's the buffer start (we can't go below that).
		if(searchPosition == m_bufferStart)
		{
			if(i == 0)
			{
				// Account the current character for the total size.
				textSize += 1;

				// We found the node we were are looking for.
				break;
			}
			else
			{
				// Return empty string.
				return std::string();
			}
		}

		// Null characters is how we determine the end of text nodes.
		if(m_buffer[searchPosition] == '\0')
		{
			if(i == 0)
			{
				// Return the the pointer after the null character (move search position back up).
				searchPosition += 1;

				if(searchPosition == m_bufferSize)
				{
					searchPosition = 0;
				}

				// We found the next node we are looking for.
				break;
			}
			else
			{
				// Not the node we are looking for.
				--i;

				// Reset text size (account for this null character).
				textSize = -1;
			}
		}

		// Advance further down.
		--searchPosition;

		if(searchPosition == -1)
		{
			searchPosition = m_bufferSize - 1;
		}

		// Increase text size counter.
		++textSize;
	}

	//
	// Copy text to a temporary string.
	//

	// Reserve memory to avoid extra allocations.
	text.reserve(textSize);

	// Copy characters to the text string.
	while(true)
	{
		// We don't want the null character for the text string.
 		if(m_buffer[searchPosition] == '\0')
			break;

		// Push back the character at the end of text string.
		text.push_back(m_buffer[searchPosition]);

		// Advance further up.
		++searchPosition;

		if(searchPosition == m_bufferSize)
		{
			searchPosition = 0;
		}
	}

	// Check if we copied more than we initially allocated.
	assert(text.size() == textSize);

	return text;
}
