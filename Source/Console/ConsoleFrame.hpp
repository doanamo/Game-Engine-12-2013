#pragma once

#include "Precompiled.hpp"
#include "Font.hpp"

//
// Console Frame
//

enum class InputCursor
{
	Invalid,

	Begin,
	End,
	Left,
	Right,
};

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
	void MoveInputCursor(InputCursor move);
	void EraseLastInputCharacter();
	void AppendInput(const char* text);
	void ExecuteInput();

	void Draw(const glm::mat4& transform);

	bool IsOpen() const
	{
		return m_open;
	}

private:
	Font m_font;

	std::string m_input;
	int m_cursorPosition;

	bool m_open;
	bool m_initialized;
};
