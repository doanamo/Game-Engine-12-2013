#include "Precompiled.hpp"
#include "ConsoleFrame.hpp"

ConsoleFrame::ConsoleFrame() :
	m_valid(false),
	m_open(false)
{
}

ConsoleFrame::~ConsoleFrame()
{
	Cleanup();
}

bool ConsoleFrame::Initialize()
{
	Cleanup();

	// Create a shader.
	if(!m_shader.Load(Context::workingDir + "Data/shader.glsl"))
	{
		Cleanup();
		return false;
	}

	// Create a vertex buffer.
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
	};

	glm::vec3 color = glm::vec3(0.3f, 0.3f, 0.3f);

	Vertex vertexData[] =
	{
		{ glm::vec3(-1.0f,  0.0f, 0.0f), color },
		{ glm::vec3( 1.0f,  0.0f, 0.0f), color },
		{ glm::vec3( 1.0f,  1.0f, 0.0f), color },

		{ glm::vec3( 1.0f,  1.0f, 0.0f), color },
		{ glm::vec3(-1.0f,  1.0f, 0.0f), color },
		{ glm::vec3(-1.0f,  0.0f, 0.0f), color },
	};

	if(!m_vertexBuffer.Initialize(sizeof(Vertex), StaticArraySize(vertexData), &vertexData[0]))
	{
		Cleanup();
		return false;
	}

	// Create a vertex input.
	VertexAttribute vertexAttributes[] =
	{
		{ 0, &m_vertexBuffer, VertexAttributeTypes::Float3 },
		{ 1, &m_vertexBuffer, VertexAttributeTypes::Float3 },
	};

	if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
	{
		Cleanup();
		return false;
	}

	// Set valid state.
	m_valid = true;

	return true;
}

void ConsoleFrame::Cleanup()
{
	m_vertexInput.Cleanup();
	m_vertexBuffer.Cleanup();
	m_shader.Cleanup();

	m_valid = false;
	m_open = false;
}

void ConsoleFrame::Open()
{
	assert(m_valid);

	m_open = true;
}

void ConsoleFrame::Close()
{
	assert(m_valid);

	m_open = false;
}

void ConsoleFrame::Toggle()
{
	assert(m_valid);

	m_open = !m_open;
}

void ConsoleFrame::Draw()
{
	assert(m_valid);

	if(m_open)
	{
		glUseProgram(m_shader.GetHandle());
		glBindVertexArray(m_vertexInput.GetHandle());

		glDrawArrays(GL_TRIANGLES, 0, m_vertexBuffer.GetElementCount());

		glBindVertexArray(0);
		glUseProgram(0);
	}
}
