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
	m_bufferLimit(0),
	m_bufferBegin(0),
	m_bufferEnd(-1),
	m_initialized(false)
{
	// m_bufferEnd must start at -1 so we can avoid an extra check on the first write.
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
	m_buffer = new char[m_bufferSize];

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
	m_bufferLimit = 0;
	m_bufferBegin = 0;
	m_bufferEnd = -1;

	m_initialized = false;
}

void ConsoleHistory::Write(const char* text)
{
	if(!m_initialized)
		return;

	//
	// Two strings in a buffer: "Hello" and "world" (with null terminators).
	//
	//      End  Limit
	//       |     |
	// [world0Hello0000] Buffer
	//        |       |
	//       Begin  Size-1
	//

	//
	// Calculate variables.
	//

	// Calculate text size, not length (could be UTF-8).
	std::size_t textSize = strlen(text);

	// Calculate write size and include the null character.
	int writeSize = textSize + 1;

	// Check if the text will fit in the buffer at all.
	if(writeSize > m_bufferSize)
		return;

	// Calculate buffer position where we will write the text.
	int writeBegin = m_bufferEnd + 1;

	// Check if the text will fit until the buffer ends.
	if(writeBegin + writeSize > m_bufferSize)
	{
		// Check if we are going to overwrite buffer beginning.
		if(m_bufferBegin >= writeBegin)
		{
			// Move buffer begin to the beginning of the buffer (where last text node starts).
			m_bufferBegin = 0;

			// Move buffer limit to current buffer end.
			m_bufferLimit = m_bufferEnd;
		}

		// Move write begin to the beginning of the buffer.
		writeBegin = 0;
	}

	// Calculate write end (location of written null character).
	int writeEnd = writeBegin + writeSize - 1;

	//
	// Write text to the buffer.
	//

	// Check if we will overwrite any nodes.
	bool nodeOverwritten = false;
	bool fullOverwrite = false;

	if(writeBegin <= m_bufferBegin && m_bufferBegin <= writeEnd)
	{
		nodeOverwritten = true;

		// Check if we are doing a full overwrite (if the last character we are about to overwrite is another null).
		if(m_buffer[writeEnd] == '\0')
		{
			fullOverwrite = true;
		}
	}

	// Write the entire text to buffer (include null character).
	memcpy(&m_buffer[writeBegin], &text[0], sizeof(char) * writeSize);

	// Move the buffer end to the write end (this is the position of new null character).
	m_bufferEnd = writeEnd;

	// Move the buffer limit if needed.
	if(writeEnd > m_bufferLimit)
	{
		m_bufferLimit = writeEnd;
	}

	//
	// Discard text node we possibly overwritten.
	//

	// Close the node we partially overwritten.
	if(nodeOverwritten)
	{
		if(fullOverwrite)
		{
			// We already have to location of the new buffer start.
			m_bufferBegin = writeEnd + 1;
		}
		else
		{
			// Search after the text (including null) we wrote.
			int searchPosition = writeEnd + 1;

			// Wrap the search position if it's past the buffer limit.
			if(searchPosition > m_bufferLimit)
			{
				searchPosition = 0;
			}
		
			// Find the null character and close the node.
			bool foundNull = false;

			while(!foundNull)
			{
				// Look for null character of the text string we overwritten.
				if(m_buffer[searchPosition] == '\0')
				{
					foundNull = true;

					// Check if we overwritten a text positioned next to the limit of the buffer.
					// If yes, move the buffer limit to where we ended last write.
					if(searchPosition == m_bufferLimit)
					{
						m_bufferLimit = writeEnd;
					}
				}

				// Advance the search position.
				++searchPosition;

				// Wrap if needed.
				if(searchPosition > m_bufferLimit)
				{
					searchPosition = 0;
				}
			}

			// Set new buffer begin position.
			m_bufferBegin = searchPosition;
		}

		// Check if we went past the buffer limit.
		if(m_bufferBegin > m_bufferLimit)
		{
			m_bufferBegin = 0;
		}
	}
}

void ConsoleHistory::Clear()
{
	if(!m_initialized)
		return;

	// No need to clear the buffer, just set the initial variables.
	m_bufferLimit = 0;
	m_bufferBegin = 0;
	m_bufferEnd = -1;
}

const char* ConsoleHistory::GetText(int index) const
{
	if(IsEmpty())
		return nullptr;

	if(index < 0)
		return nullptr;

	//
	// Search for text node by index.
	//

	int i = index;

	int searchPosition = m_bufferEnd - 1;

	while(true)
	{
		// Checks if it's the buffer begin (we can't go below that).
		if(searchPosition == m_bufferBegin)
		{
			if(i == 0)
			{
				// We found the node we were are looking for.
				break;
			}
			else
			{
				// Return empty string.
				return "";
			}
		}

		// Null characters is how we determine the end of text nodes.
		if(m_buffer[searchPosition] == '\0')
		{
			if(i == 0)
			{
				// Return the the pointer after the null character (move search position back up).
				searchPosition += 1;

				if(searchPosition == m_bufferLimit + 1)
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
			}
		}

		// Advance further down.
		--searchPosition;

		if(searchPosition == -1)
		{
			searchPosition = m_bufferLimit;
		}
	}

	return &m_buffer[searchPosition];
}
