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

	bool Initialize(int outputSize);
	void Cleanup();

	void Write(const char* text);
	void Clear();

	const char* GetText(int index) const;

	bool IsEmpty() const
	{
		return m_outputHistory.empty();
	}

	bool IsValid() const
	{
		return m_initialized;
	}

private:
	typedef std::deque<std::string> StringQueue;

	StringQueue	m_outputHistory;
	int			m_outputSize;

	bool m_initialized;
};
