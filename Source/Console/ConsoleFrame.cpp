#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"
#include "ShapeRenderer.hpp"
#include "TextRenderer.hpp"

ConsoleFrame::ConsoleFrame() :
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

	if(Context::shapeRenderer == nullptr)
	{
		return false;
	}

	if(Context::textRenderer == nullptr)
	{
		return false;
	}

	m_initialized = true;

	return true;
}

void ConsoleFrame::Cleanup()
{
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

void ConsoleFrame::Toggle()
{
	if(!m_initialized)
		return;

	m_open = !m_open;
}

void ConsoleFrame::Draw(const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(m_open)
	{
		ShapeRenderer::Quad quad;
		quad.position.x = 0.0f;
		quad.position.y = 288.0f;
		quad.size.x = 1024.0f;
		quad.size.y = 288.0f;
		quad.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.8f);
		quad.texture = nullptr;

		Context::shapeRenderer->DrawQuads(&quad, 1, transform);
	}
}
