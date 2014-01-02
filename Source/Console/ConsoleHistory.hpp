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

	bool Initialize(int outputHistorySize);
	void Cleanup();

	void Write(const char* text);
	void Clear();

	const char* GetText(int index) const;

	bool IsEmpty() const
	{
		return m_outputHistory.empty();
	}

private:
	typedef std::deque<std::string> StringQueue;

	StringQueue	m_outputHistory;
	int			m_outputHistorySize;

	bool m_initialized;
};
