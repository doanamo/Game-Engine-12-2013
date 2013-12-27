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

	void ClearInput();
	void EraseLastInputCharacter();
	void AppendInput(const char* text);
	void ExecuteInput();

	void Draw(const glm::mat4& transform);

	bool IsOpen() const
	{
		return m_open;
	}

private:
	std::string m_input;

	Font m_font;

	bool m_open;
	bool m_initialized;
};
