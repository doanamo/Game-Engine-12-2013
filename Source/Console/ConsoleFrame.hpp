#pragma once

#include "Precompiled.hpp"

//
// Console Frame
//

class ConsoleFrame
{
public:
	ConsoleFrame();
	~ConsoleFrame();

	bool Initialize();
	void Cleanup();

	void Open();
	void Close();
	void Toggle();

	void Draw(const glm::mat4& transform);

private:
	bool m_open;
	bool m_initialized;
};
