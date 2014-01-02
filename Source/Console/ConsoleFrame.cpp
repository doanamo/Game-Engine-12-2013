#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"
#include "ConsoleSystem.hpp"
#include "ConsoleHistory.hpp"
#include "ShapeRenderer.hpp"
#include "TextRenderer.hpp"

namespace
{
	const int ConsoleSize = 16;
}

ConsoleFrame::ConsoleFrame() :
	m_font(),
	m_input(""),
	m_cursorPosition(0),
	m_open(false),
	m_initialized(false)
{
}

ConsoleFrame::~ConsoleFrame()
{
	Cleanup();
}

bool ConsoleFrame::Initialize()
{
	Cleanup();

	// Load font file.
	if(!m_font.Load(Context::WorkingDir() + "Data/Fonts/SourceSansPro.ttf", 16, 512, 512))
	{
		Cleanup();
		return false;
	}

	// Cache ASCII character set.
	m_font.CacheASCII();

	m_initialized = true;

	return true;
}

void ConsoleFrame::Cleanup()
{
	m_font.Cleanup();

	m_input.clear();
	m_cursorPosition = 0;

	m_open = false;
	m_initialized = false;
}

void ConsoleFrame::Open()
{
	if(!m_initialized)
		return;

	m_open = true;
}

void ConsoleFrame::Close()
{
	if(!m_initialized)
		return;

	m_open = false;
}

void ConsoleFrame::Process(const SDL_Event& event)
{
	if(!m_initialized)
		return;

	switch(event.type)
	{
	case SDL_KEYDOWN:
		if(event.key.keysym.scancode == SDL_SCANCODE_GRAVE && event.key.repeat == 0)
		{
			if(!m_open)
			{
				// Open console frame.
				m_open = true;

				// Start text input.
				SDL_StartTextInput();
			}
			else
			{
				// Close console frame.
				m_open = false;

				// End text input.
				SDL_StopTextInput();
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
		{
			if(m_open)
			{
				// Move cursor to the left.
				m_cursorPosition = std::max(0, m_cursorPosition - 1);
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			if(m_open)
			{
				// Move cursor to the right.
				int inputLength = utf8::distance(m_input.begin(), m_input.end());
				m_cursorPosition = std::min(m_cursorPosition + 1, inputLength);
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_HOME)
		{
			if(m_open)
			{
				// Move cursor to the beginning.
				m_cursorPosition = 0;
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_END)
		{
			if(m_open)
			{
				// Move cursor to the end.
				int inputLength = utf8::distance(m_input.begin(), m_input.end());
				m_cursorPosition = inputLength;
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_RETURN && event.key.repeat == 0)
		{
			if(m_open)
			{
				// Execute input.
				Context::ConsoleSystem().Execute(m_input);

				// Clear input.
				this->ClearInput();
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
		{
			if(m_open)
			{
				// Erase a character on the left of the cursor.
				if(m_cursorPosition > 0)
				{
					// Determine part of the string we want to erase.
					auto eraseBegin = m_input.begin();
					utf8::unchecked::advance(eraseBegin, m_cursorPosition - 1);

					auto eraseEnd = eraseBegin;
					utf8::unchecked::advance(eraseEnd, 1);

					// Erase a character.
					m_input.erase(eraseBegin, eraseEnd);

					// Move cursor back.
					m_cursorPosition -= 1;
				}
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_DELETE)
		{
			if(m_open)
			{
				// Erase a character on the right of the cursor.
				int inputLength = utf8::distance(m_input.begin(), m_input.end());

				if(m_cursorPosition < inputLength)
				{
					// Determine part of the string we want to erase.
					auto eraseBegin = m_input.begin();
					utf8::unchecked::advance(eraseBegin, m_cursorPosition);

					auto eraseEnd = eraseBegin;
					utf8::unchecked::advance(eraseEnd, 1);

					// Erase a character.
					m_input.erase(eraseBegin, eraseEnd);
				}
			}
		}
		else
		if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && event.key.repeat == 0)
		{
			if(m_open)
			{
				// Clear console input.
				this->ClearInput();
			}
		}
		break;

	case SDL_TEXTINPUT:
		if(m_open)
		{
			// Get the character string.
			const char* text = event.text.text;
			assert(text != nullptr);

			// Determine cursor position on the input buffer.
			auto insertBegin = m_input.begin();
			utf8::unchecked::advance(insertBegin, m_cursorPosition);

			// Insert entered characters at the cursor position.
			m_input.insert(insertBegin, text, text + strlen(text));
			m_cursorPosition += 1;
		}
		break;
	}
}

void ConsoleFrame::Draw(const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(m_open)
	{
		float windowHeight = 576.0f;
		float consoleExtra = 1.0f;
		float consoleSize = (float)ConsoleSize * m_font.GetLineSpacing();
		float consolePosition = windowHeight - consoleSize;

		// Draw console background.
		ShapeRenderer::Quad quad;
		quad.position.x = 0.0f;
		quad.position.y = consolePosition - consoleExtra;
		quad.size.x = 1024.0f;
		quad.size.y = consoleSize + consoleExtra;
		quad.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.85f);
		quad.texture = nullptr;

		Context::ShapeRenderer().DrawQuads(&quad, 1, transform);

		// Draw console text.
		for(int i = 0; i < ConsoleSize - 1; ++i)
		{
			const char* text = Context::ConsoleHistory().GetText(i);

			if(text == nullptr)
				break;

			TextRenderer::DrawInfo info;
			info.font = &m_font;
			info.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			info.position.x = 5.0f;
			info.position.y = consolePosition + (i + 2) * m_font.GetLineSpacing();
			info.size.x = 1024.0f - 1.0f;
			info.size.y = 0.0f;

			Context::TextRenderer().Draw(info, transform, text);
		}

		// Draw console input.
		{
			std::string inputText = "> ";
			inputText += m_input;

			TextRenderer::DrawInfo info;
			info.font = &m_font;
			info.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			info.position.x = 5.0f;
			info.position.y = consolePosition + m_font.GetLineSpacing();
			info.size.x = 1024.0f - 1.0f;
			info.size.y = 0.0f;
			info.cursorIndex = 2 + m_cursorPosition;

			Context::TextRenderer().Draw(info, transform, inputText.c_str());
		}
	}
}

void ConsoleFrame::ClearInput()
{
	m_input.clear();
	m_cursorPosition = 0;
}
