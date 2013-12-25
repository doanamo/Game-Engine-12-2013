#pragma once

#include "Precompiled.hpp"
#include "Font.hpp"

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
	Font m_font;

	bool m_open;
	bool m_initialized;
};
