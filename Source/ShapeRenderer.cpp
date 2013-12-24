#include "Precompiled.hpp"
#include "ShapeRenderer.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a shape renderer! "

	// Minimum buffer size so certain shaped won't fail rendering.
	const int MinimumBufferSize = 4;
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
	const int LineVertices = 2;

	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Line& line = data[i];

		// Cretae line vertices.
		Vertex lineVertices[LineVertices] =
		{
			{ line.begin, glm::vec2(0.0f, 0.0f), line.color },
			{ line.end,   glm::vec2(0.0f, 0.0f), line.color },
		};

		// Copy lines to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &lineVertices[0], sizeof(Vertex) * LineVertices);

		verticesBatched += LineVertices;

		// Draw if we reached buffer size or last data element.
		if(m_bufferSize - verticesBatched < LineVertices || i == count - 1)
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

void ShapeRenderer::DrawRectangles(const Rectangle* data, int count, const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(data == nullptr)
		return;

	if(count <= 0)
		return;

	assert(m_bufferSize >= 4);

	// Bind rendering states.
	glUseProgram(m_shader.GetHandle());
	glBindVertexArray(m_vertexInput.GetHandle());

	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);

	// Batch lines and draw them.
	const int RectangleVertices = 4;

	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Rectangle& rectangle = data[i];

		// Cretae rectangle vertices.
		glm::vec4 rect(rectangle.position, rectangle.position + rectangle.size);

		Vertex rectangleVertices[RectangleVertices] =
		{
			{ glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.z, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.z, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },
		};

		// Copy lines to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &rectangleVertices[0], sizeof(Vertex) * RectangleVertices);

		verticesBatched += RectangleVertices;

		// Draw if we reached buffer size or last data element.
		if(m_bufferSize - verticesBatched < RectangleVertices || i == count - 1)
		{
			// Update vertex buffer.
			m_vertexBuffer.Update(m_bufferData);

			// Draw rectangles.
			glDrawArrays(GL_LINE_LOOP, 0, verticesBatched);

			// Reset batch counter.
			verticesBatched = 0;
		}
	}

	// Unbind render states.
	glBindVertexArray(0);
	glUseProgram(0);
}
