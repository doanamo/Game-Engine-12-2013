#include "Precompiled.hpp"
#include "ShapeRenderer.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a shape renderer! "

	// Minimum buffer size so certain shaped won't fail rendering.
	const int MinimumBufferSize = 2;
}

ShapeRenderer::ShapeRenderer() :
	m_bufferData(nullptr),
	m_bufferSize(0),
	m_initialized(true)
{
}

ShapeRenderer::~ShapeRenderer()
{
}

bool ShapeRenderer::Initialize(int bufferSize)
{
	Cleanup();
	
	// Validate arguments.
	if(bufferSize <= MinimumBufferSize)
	{
		Log() << LogInitializeError() << "Invalid argument - \"bufferSize\" is invalid.";
		return false;
	}

	m_bufferSize = bufferSize;

	// Allocate buffer memory.
	m_bufferData = new Vertex[m_bufferSize];

	// Load a shader.
	if(!m_shader.Load(Context::workingDir + "Data/Shaders/Shape.glsl"))
	{
		Log() << LogInitializeError() << "Failed to load a shader.";
		Cleanup();
		return false;
	}

	// Create a vertex buffer.
	if(!m_vertexBuffer.Initialize(sizeof(Vertex), m_bufferSize, nullptr, GL_DYNAMIC_DRAW))
	{
		Log() << LogInitializeError() << "Failed to create a vertex buffer.";
		Cleanup();
		return false;
	}

	// Create a vertex input.
	VertexAttribute vertexAttributes[3] =
	{
		{ 0, &m_vertexBuffer, VertexAttributeTypes::Float2 },
		{ 1, &m_vertexBuffer, VertexAttributeTypes::Float2 },
		{ 2, &m_vertexBuffer, VertexAttributeTypes::Float4 },
	};

	if(!m_vertexInput.Initialize(&vertexAttributes[0], StaticArraySize(vertexAttributes)))
	{
		Log() << LogInitializeError() << "Failed to create a vertex input.";
		Cleanup();
		return false;
	}

	// Return success.
	m_initialized = true;

	return true;
}

void ShapeRenderer::Cleanup()
{
	delete[] m_bufferData;
	m_bufferData = nullptr;

	m_bufferSize = 0;

	m_shader.Cleanup();
	m_vertexBuffer.Cleanup();
	m_vertexInput.Cleanup();

	m_initialized = false;
}

void ShapeRenderer::DrawLines(const Line* data, int count, const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(data == nullptr)
		return;

	if(count <= 0)
		return;

	assert(m_bufferSize >= 2);

	// Bind rendering states.
	glUseProgram(m_shader.GetHandle());
	glBindVertexArray(m_vertexInput.GetHandle());

	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);

	// Batch lines and draw them.
	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Line& line = data[i];

		// Cretae line vertices.
		Vertex lineVertices[2] =
		{
			{ line.begin, glm::vec2(0.0f, 0.0f), line.color },
			{ line.end,   glm::vec2(0.0f, 0.0f), line.color },
		};

		// Copy lines to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &lineVertices[0], sizeof(Vertex) * 2);

		verticesBatched += 2;

		// Draw if we reached buffer size or last data element.
		if(m_bufferSize - verticesBatched < 2 || i == count - 1)
		{
			// Update vertex buffer.
			m_vertexBuffer.Update(m_bufferData);

			// Draw lines.
			glDrawArrays(GL_LINES, 0, verticesBatched);

			// Reset batch counter.
			verticesBatched = 0;
		}
	}

	// Unbind render states.
	glBindVertexArray(0);
	glUseProgram(0);
}
