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
	m_input(""),
	m_font(),
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
	if(!m_font.Load(Context::workingDir + "Data/Fonts/SourceSansPro.ttf", 16, 512, 512))
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
	m_input.clear();

	m_font.Cleanup();

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

void ConsoleFrame::ClearInput()
{
	m_input.clear();
}

void ConsoleFrame::AppendInput(const char* text)
{
	m_input.append(text);
}

void ConsoleFrame::ExecuteInput()
{
	// Execute input.
	Context::consoleSystem->Execute(m_input);

	// Clear input.
	ClearInput();
}

void ConsoleFrame::Draw(const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(m_open)
	{
		float windowHeight = 576.0f;
		float consoleHeight = (float)ConsoleSize * m_font.GetLineSpacing();

		// Draw console background.
		ShapeRenderer::Quad quad;
		quad.position.x = 0.0f;
		quad.position.y = windowHeight - consoleHeight;
		quad.size.x = 1024.0f;
		quad.size.y = consoleHeight;
		quad.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.85f);
		quad.texture = nullptr;

		Context::shapeRenderer->DrawQuads(&quad, 1, transform);

		// Draw console text.
		for(int i = 0; i < ConsoleSize - 1; ++i)
		{
			std::string text = Context::consoleHistory->GetText(i);

			Context::textRenderer->Draw(&m_font, glm::vec2(5.0f, quad.position.y + (i + 2) * m_font.GetLineSpacing()), 1024.0f - 1.0f, transform, text.c_str());
		}

		// Draw console input.
		std::string inputText = "> ";
		inputText += m_input;

		Context::textRenderer->Draw(&m_font, glm::vec2(5.0f, quad.position.y + m_font.GetLineSpacing()), 1024.0f - 1.0f, transform, inputText.c_str());
	}
}
