#pragma once

#include "Precompiled.hpp"

//
// Console History
//

class ConsoleHistory
{
public:
	ConsoleHistory();
	~ConsoleHistory();

	bool Initialize(int bufferSize);
	void Cleanup();

	void Write(const wchar_t* text);

	bool IsEmpty() const
	{
		return m_bufferStart == -1;
	}

private:
	// Text ring buffer.
	wchar_t* m_buffer;
	int m_bufferSize;

	int m_bufferStart;
	int m_bufferEnd;

	bool m_initialized;
};
