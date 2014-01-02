#include "Precompiled.hpp"
#include "ShapeRenderer.hpp"
#include "Graphics/Texture.hpp"

namespace
{
	// Log error messages.
	#define LogInitializeError() "Failed to initialize a shape renderer! "

	// Minimum buffer size so certain shaped won't fail rendering.
	const int MinimumBufferSize = 6;
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
	if(!m_shader.Load(Context::WorkingDir() + "Data/Shaders/Shape.glsl"))
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

	// Minimum buffer size needed.
	const int ShapeVerticeCount = 2;

	assert(m_bufferSize >= ShapeVerticeCount);

	// Bind rendering states.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Context::TextureBlank().GetHandle());

	glUseProgram(m_shader.GetHandle());
	glBindVertexArray(m_vertexInput.GetHandle());

	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);

	// Batch shapes and draw them.
	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Line& line = data[i];

		// Create shapes vertices.
		Vertex shapeVertices[ShapeVerticeCount] =
		{
			{ line.begin, glm::vec2(0.0f, 0.0f), line.color },
			{ line.end,   glm::vec2(0.0f, 0.0f), line.color },
		};

		// Copy vertices to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

		verticesBatched += ShapeVerticeCount;

		// Draw if we reached buffer size or last data element.
		if(m_bufferSize - verticesBatched < ShapeVerticeCount || i == count - 1)
		{
			// Update vertex buffer.
			m_vertexBuffer.Update(m_bufferData);

			// Draw shapes.
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

	// Minimum buffer size needed.
	const int ShapeVerticeCount = 8;

	assert(m_bufferSize >= ShapeVerticeCount);

	// Bind rendering states.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Context::TextureBlank().GetHandle());

	glUseProgram(m_shader.GetHandle());
	glBindVertexArray(m_vertexInput.GetHandle());

	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);

	// Batch shapes and draw them.
	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Rectangle& rectangle = data[i];

		// Create shape vertices.
		glm::vec4 rect(rectangle.position, rectangle.position + rectangle.size);

		Vertex shapeVertices[ShapeVerticeCount] =
		{
			{ glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.z, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },

			{ glm::vec2(rect.z, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.z, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },

			{ glm::vec2(rect.z, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },

			{ glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 0.0f), rectangle.color },
			{ glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), rectangle.color },
		};

		// Copy vertices to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

		verticesBatched += ShapeVerticeCount;

		// Draw if we reached buffer size or last data element.
		if(m_bufferSize - verticesBatched < ShapeVerticeCount || i == count - 1)
		{
			// Update vertex buffer.
			m_vertexBuffer.Update(m_bufferData);

			// Draw shapes.
			glDrawArrays(GL_LINES, 0, verticesBatched);

			// Reset batch counter.
			verticesBatched = 0;
		}
	}

	// Unbind render states.
	glBindVertexArray(0);
	glUseProgram(0);
}

void ShapeRenderer::DrawQuads(const Quad* data, int count, const glm::mat4& transform)
{
	if(!m_initialized)
		return;

	if(data == nullptr)
		return;

	if(count <= 0)
		return;

	// Minimum buffer size needed.
	const int ShapeVerticeCount = 6;

	assert(m_bufferSize >= ShapeVerticeCount);

	// Bind rendering states.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(m_shader.GetHandle());
	glBindVertexArray(m_vertexInput.GetHandle());

	glUniformMatrix4fv(m_shader.GetUniform("vertexTransform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1i(m_shader.GetUniform("texture"), 0);

	// Batch shapes and draw them.
	const Texture* currentTexture = nullptr;
	int verticesBatched = 0;

	for(int i = 0; i < count; ++i)
	{
		const Quad& quad = data[i];

		// Create shape vertices.
		glm::vec4 rect(quad.position, quad.position + quad.size);

		Vertex shapeVertices[ShapeVerticeCount] =
		{
			{ glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), quad.color },
			{ glm::vec2(rect.z, rect.y), glm::vec2(1.0f, 0.0f), quad.color },
			{ glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), quad.color },

			{ glm::vec2(rect.z, rect.w), glm::vec2(1.0f, 1.0f), quad.color },
			{ glm::vec2(rect.x, rect.y), glm::vec2(0.0f, 0.0f), quad.color },
			{ glm::vec2(rect.x, rect.w), glm::vec2(0.0f, 1.0f), quad.color },
		};

		// Copy vertices to a temporary buffer.
		memcpy(&m_bufferData[verticesBatched], &shapeVertices[0], sizeof(Vertex) * ShapeVerticeCount);

		verticesBatched += ShapeVerticeCount;

		// Determine if we have to draw the batch.
		bool drawBatch = false;

		if(m_bufferSize - verticesBatched < ShapeVerticeCount)
		{
			// We reached the buffer size.
			drawBatch = true;
		}
		else
		if(i == count - 1)
		{
			// We reached the last element.
			drawBatch = true;
		}
		else
		if(quad.texture != data[i + 1].texture)
		{
			// Texture is going to change, we have to draw.
			drawBatch = true;
		}

		// Draw the batch if needed.
		if(drawBatch)
		{
			// Update vertex buffer.
			m_vertexBuffer.Update(m_bufferData);

			// Bind the texture.
			glActiveTexture(GL_TEXTURE0);

			if(quad.texture != nullptr)
			{
				glBindTexture(GL_TEXTURE_2D, quad.texture->GetHandle());
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, Context::TextureBlank().GetHandle());
			}

			// Draw shapes.
			glDrawArrays(GL_TRIANGLES, 0, verticesBatched);

			// Reset batch counter.
			verticesBatched = 0;
		}
	}

	// Unbind render states.
	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}
