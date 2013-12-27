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

	void Write(const char* text);

	const char* GetText(int index) const;

	bool IsEmpty() const
	{
		return m_bufferBegin == m_bufferEnd;
	}

private:
	// Text ring buffer.
	char* m_buffer;

	int m_bufferSize;
	int m_bufferLimit;
	int m_bufferBegin;
	int m_bufferEnd;

	bool m_initialized;
};
